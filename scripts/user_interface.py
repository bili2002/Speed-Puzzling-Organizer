import matplotlib.pyplot as plt
from datetime import date
from datetime import datetime
from enum import Enum
import config_reader
import polars as pl
import numpy as np
import subprocess
import shutil
import sys
import os

import process_image

METADATA_PATH = config_reader.get_image_metadata_path()
IMAGE_PATH = config_reader.get_image_raw_path()
PUZZLE_DATA = config_reader.get_puzzle_data()
BACKUPS = config_reader.get_database_backups_path()
ANALYZOR = config_reader.get_analyzor()

DIFF_RESCALE = 0.1

class ENTRY(Enum):
    name = "name"
    time = "time"
    pieces = "pieces"
    brand = "brand"
    tags = "tags"

DATE = "date"


def readDF(name):
    df = pl.read_json(name)
    return df


def writeDF(df, name):
    shutil.copyfile(name, f"{BACKUPS}/{str(datetime.now())}.json")
    df.write_json(name)


def createNewDF(values):
    return pl.DataFrame(
        data = [values + [date.today()]],
        schema = [e.value for e in ENTRY] + [DATE],
        orient = 'row'
    )


def createHistoricalDF(values):
    return pl.DataFrame(
        data = [values],
        schema = [e.value for e in ENTRY] + [DATE],
        orient = 'row'
    )


def vertifyEntry(entry, hasDate):
    try:
        entry[1] = int(entry[1])
        entry[2] = int(entry[2])

        entry[4] = list(map(str, entry[4][1:-1].replace(' ', '').split(',')))
        
        if (hasDate):
            temp = map(int, entry[5].split('-'))
            entry[5] = date(*temp)
    except:
        raise "An entry should be in the format [name: str] [time: int] [pieces: int] [brand: str] [tags: list] {[date: date]}"


def addEntry(argv_begin):
    new_data = sys.argv[argv_begin : len(sys.argv)]
    hasDate = len(sys.argv) - argv_begin > len(ENTRY)

    vertifyEntry(new_data, hasDate)

    df_base = readDF(PUZZLE_DATA)

    if (hasDate):
        df_new = createHistoricalDF(new_data)
    else:
        df_new = createNewDF(new_data)

    df_base = df_base.vstack(df_new)
    writeDF(df_base, PUZZLE_DATA)


def deleteEntry(argv_begin):
    row = int(sys.argv[argv_begin])

    df = readDF(PUZZLE_DATA)
    df = df[:row].vstack(df[row + 1:])

    writeDF(df, PUZZLE_DATA)
    

def showAll():
    df = readDF(PUZZLE_DATA)
    pl.Config(tbl_rows=len(df))
    print(df.with_row_count())


def showFastest():
    df = readDF(PUZZLE_DATA)
    print(df.sort(ENTRY.time.value).head(10))


def showSlowest():
    df = readDF(PUZZLE_DATA)
    print(df.sort(ENTRY.time.value, descending = True).head(10))


def filterByByTags(argv_begin):
    def intersection(lst1, lst2):
        return list(set(lst1) & set(lst2))
    
    df = readDF(PUZZLE_DATA)

    if argv_begin == len(sys.argv):
        return df
    
    tags = sys.argv[argv_begin : len(sys.argv)]

    try:
        excluding_idx = tags.index("exclude")
    except:
        excluding_idx = len(tags)
    
    tags_include = tags[: excluding_idx]
    tags_exclude = tags[excluding_idx + 1:]

    df = df.filter(pl.col(ENTRY.tags.value).apply(
        lambda x: 
            len(intersection(x, tags_include)) > 0 and 
            len(intersection(x, tags_exclude)) == 0
        ))
    
    return df


def getAllTags(df):
    tags = df[ENTRY.tags.value].drop_nulls()
    return np.array(list(set().union(*tags)))


def groupByTag(df):
    return df.select(ENTRY.tags.value, ENTRY.time.value).explode(ENTRY.tags.value).groupby(ENTRY.tags.value)


def getAverageTime(df):
    return df.mean(ENTRY.time.value)


def plotMeanTimeByTag(df):
    df = groupByTag(df).mean().drop_nulls()

    tags = np.array(df[ENTRY.tags.value])
    times = np.array(df[ENTRY.time.value])

    plt.bar(tags, times, color = 'tomato')
    plt.show()


def plotCountByTag(df):
    df = groupByTag(df).count().drop_nulls()

    tags = np.array(df[ENTRY.tags.value])
    count = np.array(df["count"])

    plt.pie(count, labels = tags)
    plt.show()
    

def plotTimesByDate(df):
    df = df.sort(DATE)

    times = np.array(df[ENTRY.time.value])
    dates = np.array(df[DATE])

    plt.plot(dates, times)
    plt.show()


def findImage(name):
    files = os.listdir(IMAGE_PATH)
    
    for file in files:
        if name in file:
            return f"{file}"
        
    return None


def evaluate(name):
    print(name)

    file = findImage(name)
    if (file == None):
        return None
    
    process_image.create_metadata(file) 

    image_name = os.path.splitext(name)[0]
    metadata_file = f"{METADATA_PATH}/{image_name}.ppm"

    output = subprocess.check_output(
        [f"./{ANALYZOR} {metadata_file} 500"], 
        shell=True
    ).decode()

    return float(output) * DIFF_RESCALE


def evaluateAll(shouldPlot):
    df = readDF(PUZZLE_DATA)
    df = df.with_columns(
        pl.col("name").apply(lambda x: evaluate(x)).alias("difficulty")
    ).sort(ENTRY.time.value)

    times = np.array(df[ENTRY.time.value])
    difficulties = np.array(df["difficulty"])

    a, b = np.polyfit(times, difficulties, 1)

    if shouldPlot:
        plt.scatter(times, difficulties, color='blue')
        plt.plot(times, a * times + b, color='steelblue')

        plt.show()

        s = sum([(a * time + b - difficulty) ** 2 for time, difficulty in zip(times, difficulties)])

        print (f"{s} error")

    return a, b, s


def evaluateNew(argv_begin):
    a, b, _ = evaluateAll(False)

    name = sys.argv[argv_begin]
    difficulty = evaluate(name)

    print(f"{(difficulty - b) / a} minutes")


def plotBy(argv_begin):
    type_of_plot = sys.argv[argv_begin]
    df = filterByByTags(argv_begin + 1)

    if (type_of_plot == "meanByTag"):
        plotMeanTimeByTag(df)
    elif (type_of_plot == "countByTag"):
        plotCountByTag(df)
    elif (type_of_plot == "timesByDate"):
        plotTimesByDate(df)
    else:
        raise "No implementation for this plot type yet."


def makeProject():
    df_new = createNewDF([[]] * len(ENTRY))
    writeDF(df_new, PUZZLE_DATA)


def test():
    df = readDF(PUZZLE_DATA)
    print(df)
