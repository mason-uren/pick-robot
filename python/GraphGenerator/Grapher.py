import matplotlib.pyplot as plt

class Grapher:

    def __init__(self, df1, df2):
        self._noBag = df1
        self._withBag = df2

    def createGraphs(self):
        fig = plt.figure(1)
        ax_nb = fig.add_subplot(111)
        ax_nb.boxplot([list(self._noBag["noBag"]), self._withBag["withBag"]])
        ax_nb.set_xticklabels(['No-Bag', 'With-Bag'])

        low_thresh = self._noBag["noBag"].astype(float).mean(axis=0) - 2 * self._noBag["noBag"].astype(float).std(axis=0)
        high_thresh = self._withBag["withBag"].astype(float).mean(axis=0) + 2 * self._withBag["withBag"].astype(float).std(axis=0)
        ax_nb.axhline(color='r', linestyle='--', y=low_thresh)
        ax_nb.axhline(color='r', linestyle='--', y=high_thresh)
        fig.show()
