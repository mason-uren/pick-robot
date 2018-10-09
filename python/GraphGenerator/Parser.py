import pandas as pd
import numpy as np
import csv

from Grapher import Grapher

class Parser:

    def __init__(self, file_path):
        self._file_path = file_path
        self._rows = None

    def fileToDataFrame(self, column_name):
        self.formatData()
        df = pd.DataFrame(self._rows, columns=[column_name]).astype(float)
        if column_name == "withBag":
            df = df[df[column_name] <= 900]
        if column_name == "noBag":
            df = df[df[column_name] <= 900]
        return df

    def formatData(self):
        file = open(self._file_path, 'r')
        reader = csv.reader(file)
        self._rows = [row for row in reader if len(row) > 0]
        self._rows = [item for sublist in self._rows for item in sublist]

    def printSpecs(self, df, column_name):
        print("WITHOUT BAG: GAIN = 1, DATARATE = 860")
        print("Count: {}".format(df.shape[0]))
        print("Mean: {}".format(df[column_name].astype(float).mean(axis=0)))
        print("Min: {}".format(df[column_name].astype(float).min(axis=0)))
        print("Max: {}".format(df[column_name].astype(float).max(axis=0)))
        print("Std: {}".format(df[column_name].astype(float).std(axis=0)))
        print()

if __name__ == "__main__":
    noBag = Parser("../../Data/testingNoBag_gain1.txt") # TODO: finish file path
    withBag = Parser("../../Data/testingBag_gain1.txt")

    vac_no_bag = noBag.fileToDataFrame("noBag")
    vac_with_bag = withBag.fileToDataFrame("withBag")

    noBag.printSpecs(vac_no_bag, "noBag")
    withBag.printSpecs(vac_with_bag, "withBag")

    graph = Grapher(vac_no_bag, vac_with_bag)
    graph.createGraphs()

    print("High-Thresh: {}".format(vac_no_bag["noBag"].astype(float).mean(axis=0) - 2 * vac_no_bag["noBag"].astype(float).std(axis=0)))
    print("Low-Thresh: {}".format(vac_with_bag["withBag"].astype(float).mean(axis=0) + 2 * vac_with_bag["withBag"].astype(float).std(axis=0)))


