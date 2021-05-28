import pandas as pd
from sklearn.model_selection import train_test_split
from sklearn.naive_bayes import GaussianNB
import numpy as np
from sklearn import metrics
import matplotlib.pyplot as plt


def canBeMaster(pl, ns, nr, fc, yoc, hp, m ):
    """Read the table"""
    df = pd.read_csv('tabelle_1.csv')
    """From the table, get these fields here"""
    X = df[['Path length', 'N.Sensors', 'N.reparations', 'Fuel Consumption', 'Year of construction', 'Horse Power',
            'Mileage']]
    """This is the output to confront against"""
    y = df['Master']
    """This shit here divides the data set randonmly. YOu can trick the test_size 0.1 means 10% as test and 90 % for training  """
    X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.10)  # 70% training and 30% test
    """Implementation of the naive byas"""
    model = GaussianNB()
    """Training of the model"""
    model.fit(X_train.to_numpy(), y_train.to_numpy())
    """predicition with the test set"""
    y_pred = model.predict(X_test.to_numpy())
    """Output the accuracy"""
    print("Accuracy:", metrics.accuracy_score(y_test, y_pred) * 100)
    """Creation of the example nparray"""
    x_p = np.array([pl, ns, nr, fc, yoc, hp, m]).reshape(1, -1)
    """Predict if master"""
    ye = model.predict(x_p)
    print(ye)



if __name__ == '__main__':
    canBeMaster(594, 75, 0, 26, 2013, 500, 20)
