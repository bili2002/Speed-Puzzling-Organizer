# Speed-Puzzling-Organizer
A tool you can use to store / view / analyze your jigsaw puzzling times. Also, you can get an expected time of a new puzzle.

## Starting the Project 
```console
python .\scripts\main.py start
```

## Adding an Entry
```console
python .\scripts\main.py add name time pieces brand tags [date]
```
In this command:

`name` is the name of your puzzle.

`time` corresponds to the time spent to solve the puzzle.

`pieces` indicates the number of pieces the puzzle consists of.

`brand` is the brand of the puzzle.

`tags` are any additional metadata you want to attach to the puzzle. The tags should be in the format: `"[tag1, tag2, ...]"`.

`date` is the date of completion of the puzzle. This parameter is optional. If left empty, it will default to the current date.


## To delete entry:
```console
python .\scripts\main.py delete row
```
`row` represents the row number of the entry you want to delete.

## Displaying All Entries
```console
python .\scripts\main.py showAll
```

## Displaying Fastest Entries
```console
python .\scripts\main.py showFastest
```

## Displaying Slowest Entries
```console
python .\scripts\main.py showSlowest
```

## Plotting
```console
python .\scripts\main.py type_of_plot [tags_to_include] exclude [tags_to_exclude]
```
The parameters should be provided as follows:

`type_of_plot` - The type of the plot. Valid options are `meanByTag`, `countByTag`, `timesByDate`.

`tags_to_include` - List of tags to include in the report in the format: `tag1, tag2,....` This parameter is optional.

`tags_to_exclude` - List of tags to exclude from the report in the format: `tag1, tag2,....` This parameter is optional. Use the keyword exclude before this parameter.

## Plot Prediction Function
```console
python .\scripts\main.py evaluateAll
```

## Get New Prediction 
```console
python .\scripts\main.py evaluateNew name
```
`name` is the name of your puzzle.

