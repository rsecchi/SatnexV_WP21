#!/usr/local/bin/python3

from statistics import *
import pandas as pd
from sklearn.model_selection import train_test_split
from sklearn.ensemble import RandomForestClassifier
from sklearn import svm
from sklearn.neural_network import MLPClassifier
from sklearn.metrics import confusion_matrix, classification_report, accuracy_score
pd.set_option('mode.chained_assignment', None)

# separate data test from data
data_in = pd.read_csv('GT.csv')
data_in['label'] = data_in['label'].astype(str)

test_inputs = data_in['label'].str.contains('T')
 
data = data_in.loc[~test_inputs]
data_test = data_in.loc[test_inputs]

if not data_test.empty:
	print("Data test present")
	data_test_X = data_test.drop('label', axis=1)
	data_test_y = data_test['label'].str.replace('T', '')


rf = RandomForestClassifier(n_estimators=200)
sv = svm.SVC()
nn = MLPClassifier(hidden_layer_sizes=[10,10,10], max_iter=500)

# classifiers = [rf, sv, nn]
classifiers = [rf, sv]

for el in classifiers:
	el.res = []

for i in range(100):
	print('#',end='',flush=True);
	X = data.drop('label', axis=1)
	y = data['label']
	X, Xt, y, yt = train_test_split(X, y, test_size=0.2, random_state=i)
	for el in classifiers:
		el.fit(X, y)
		if not data_test.empty:
			Xt = data_test_X
			yt = data_test_y
		pred = el.predict(Xt)
		acc_score = accuracy_score(yt, pred)
		el.res.append(acc_score)


for el in classifiers:
	print(type(el), mean(el.res))

