#!/usr/bin/python3

from statistics import *
import pandas as pd
from sklearn.model_selection import train_test_split
from sklearn.ensemble import RandomForestClassifier
from sklearn import svm
from sklearn.neural_network import MLPClassifier
from sklearn.metrics import confusion_matrix, classification_report, accuracy_score

data = pd.read_csv('GT.csv');

rf = RandomForestClassifier(n_estimators=200)
sv = svm.SVC()
nn = MLPClassifier(hidden_layer_sizes=[10,10,10], max_iter=500)

classifiers = [rf, sv, nn]

for el in classifiers:
	el.res = []

for i in range(100):
	X = data.drop('label', axis=1)
	y = data['label']
	X, Xt, y, yt = train_test_split(X, y, test_size=0.2, random_state=i)
	for el in classifiers:
		el.fit(X, y)
		pred = el.predict(Xt)
		acc_score = accuracy_score(yt, pred)
		el.res.append(acc_score)
	
for el in classifiers:
	print(type(el), mean(el.res))

