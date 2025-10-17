import matplotlib
import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec

matplotlib.use('agg')

class ShortCircuitPlot(object):
    def __init__(self, pars):
        self.pars = pars

    def plot(self, file):
        cases = (
            dict(
                x=self.pars['t'],
                y=[self.pars['is'][0], self.pars['is'][1],
                   self.pars['is'][2], self.pars['torque']],
                title='Short Circuit Plot'),
            dict(
                x=self.pars['t'],
                y=self.pars['torque'],
                title='Short Circuit Torque Plot'))

        cols = 2
        rows = len(cases) // cols + 1
        figsize = (7, 2.0*rows)  # width, height in inches
        gs = gridspec.GridSpec(rows, cols)
        fig1 = plt.figure(num=1, figsize=figsize)
        ax = []

        row = 0
        col = 0
        ax.append(fig1.add_subplot(gs[row, col]))
        ax[-1].set_title(cases[0]['title'])
        ax[-1].grid(True)
        plt.ylabel("[A]")
        plt.xlabel("time [ms]")

        for _y in cases[0]['y']:
            ax[-1].plot(cases[0]['x'], _y)
#        ax[-1].plot(cases[0]['x'], cases[0]['y'][0])

        col = 1
        ax.append(fig1.add_subplot(gs[row, col]))
        ax[-1].set_title(cases[1]['title'])
        ax[-1].grid(True)
        plt.ylabel("[kNm]")
        plt.xlabel("time [ms]")

        ax[-1].plot(cases[0]['x'], cases[1]['y'])

        fig1.tight_layout()

        plt.savefig(file)
        plt.close()
