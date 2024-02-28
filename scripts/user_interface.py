import matplotlib.pyplot as plt
from datetime import date
from datetime import datetime
from enum import Enum
import config_reader
import polars as pl
import numpy as np
import shutil
import sys

PUZZLE_DATA = config_reader.get_puzzle_data()
BACKUPS = config_reader.get_database_backups_path()

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

    df_base = pl.concat([df_base, df_new])
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


def getValuesByTags(argv_begin):
    def intersection(lst1, lst2):
        return list(set(lst1) & set(lst2))
    
    tags = sys.argv[argv_begin : len(sys.argv)]

    df = readDF(PUZZLE_DATA)
    df = df.filter(intersection(pl.col(ENTRY.tags.value), tags))
    
    return df


def getAllTags(df):
    tags = df[ENTRY.tags.value].drop_nulls()
    return np.array(list(set().union(*tags)))


def groupByTag(df):
    return df.select(ENTRY.tags.value, ENTRY.time.value).explode(ENTRY.tags.value).groupby(ENTRY.tags.value)


def getAverageTime(df):
    return df.mean(ENTRY.time.value)


def plotMeanTimeByTag():
    df = readDF(PUZZLE_DATA)
    df = groupByTag(df).mean().drop_nulls()

    tags = np.array(df[ENTRY.tags.value])
    times = np.array(df[ENTRY.time.value])

    plt.bar(tags, times)
    plt.show()


def plotCountByTag():
    df = readDF(PUZZLE_DATA)
    df = groupByTag(df).count().drop_nulls()

    tags = np.array(df[ENTRY.tags.value])
    count = np.array(df["count"])

    plt.pie(count, labels = tags)
    plt.show()
    

def plotTimesByDate():
    df = readDF(PUZZLE_DATA)
    df = df.sort(DATE)

    times = np.array(df[ENTRY.time.value])
    dates = np.array(df[DATE])

    plt.plot(dates, times)
    plt.show()


def makeProject():
    df_new = createNewDF([[]] * len(ENTRY))
    writeDF(df_new, PUZZLE_DATA)