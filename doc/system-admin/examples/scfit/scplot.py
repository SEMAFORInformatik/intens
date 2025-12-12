#!/usr/bin/env python
import sys
import json
import io
import base64
import numpy as np
import matplotlib.pyplot as plt

if __name__ == '__main__':
    pars = json.load(sys.stdin)

    t = np.linspace(0, pars['tmax'], pars['nsamples'])
    w1 = 2*np.pi*pars['f1']
    phi = -np.pi/2
    current = pars['Io']*(np.exp(-t/pars['T']) + np.sin(w1*t + phi))
    fig, ax = plt.subplots()
    ax.plot(t, current)
    ax.plot(t, pars['Io']*np.exp(-t/pars['T']), linestyle='dashed')
    ax.grid()

    imgbuffer = io.BytesIO()
    fig.savefig(imgbuffer, format='svg')
    plt.close(fig)
    # Convert the SVG data to base64
    svgplot = base64.b64encode(imgbuffer.getvalue()).decode()
    encoding = 'data:image/svg+xml;charset=utf-8;base64,'
    result = {}
    result['time'] = t.tolist()
    result['current'] = current.tolist()
    result['plot'] = encoding + svgplot
    json.dump(result, sys.stdout)
