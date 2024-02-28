import user_interface as ui
import process_image as pi
import sys

if __name__ == "__main__":
    if (sys.argv[1] == "start"):
        ui.makeProject()

    if (sys.argv[1] == "add"):
        ui.addEntry(2)
    elif (sys.argv[1] == "delete"):
        ui.deleteEntry(2)
    elif (sys.argv[1] == "showAll"):
        ui.showAll()
    elif (sys.argv[1] == "showFastest"):
        ui.showFastest()
    elif (sys.argv[1] == "showSlowest"):
        ui.showSlowest()
    elif (sys.argv[1] == "plotMeanByTag"):
        ui.plotMeanTimeByTag()
    elif (sys.argv[1] == "plotCountByTag"):
        ui.plotCountByTag()
    elif (sys.argv[1] == "plotTimesByDate"):
        ui.plotTimesByDate()
    elif (sys.argv[1] == "evaluateAll"):
        ui.evaluateAll()
    elif (sys.argv[1] == "test"):
        ui.test()
    else: 
        raise "No command like that"
    