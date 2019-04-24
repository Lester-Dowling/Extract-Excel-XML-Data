import matplotlib.pyplot as plt
import numpy as np
dat = np.loadtxt('graph.txt',delimiter='\t')
plt.plot(dat[:,0],dat[:,1])
plt.show()
