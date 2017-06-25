import numpy

def matrix_factorization(R, P, Q, K, steps=5000, alpha=0.0002, beta=0.0):
  Q = Q.T
  for step in range(steps):
    for i in range(len(R)):
      print(i)
      for j in range(len(R[i])):
        if R[i][j] > 0:
          eij = R[i][j] - numpy.dot(P[i,:],Q[:,j])
          P[i, :] = P[i, :] + alpha * (2 * eij * Q[:, j] - beta * P[i, :])
          Q[:, j] = Q[:, j] + alpha * (2 * eij * P[i, :] - beta * Q[:, j])
          #for k in range(K):
          #  P[i][k] = P[i][k] + alpha * (2 * eij * Q[k][j] - beta * P[i][k])
          #  Q[k][j] = Q[k][j] + alpha * (2 * eij * P[i][k] - beta * Q[k][j])
    eR = numpy.dot(P,Q)
    e = 0
    for i in range(len(R)):
      for j in range(len(R[i])):
        if R[i][j] > 0:
          e = e + pow(R[i][j] - numpy.dot(P[i,:],Q[:,j]), 2)
          for k in range(K):
            e = e + (beta/2) * (pow(P[i][k],2) + pow(Q[k][j],2))
    print("iterations:", step, ",", "training error:", e)
    if e < 0.001:
      break
  return P, Q.T

data = numpy.genfromtxt('./data/ratings.csv', delimiter=',')
R = numpy.zeros((700, 170000))
for i in range(numpy.shape(data)[0]):
    if (i == 0):
        i += 1
    R[int(data[i, 0]), int(data[i, 1])] = int(data[i, 2])
P = numpy.random.rand(700, 100)
Q = numpy.random.rand(170000, 100)
matrix_factorization(R, P, Q, 100)
print(R)
