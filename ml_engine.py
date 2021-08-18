#!/usr/bin/python3

import sys
import pandas as pd
from sklearn.model_selection import train_test_split
from sklearn.ensemble import RandomForestClassifier
from sklearn.neighbors import KNeighborsClassifier
from sklearn import svm
from sklearn.neural_network import MLPClassifier
from sklearn.metrics import confusion_matrix, classification_report 
from sklearn.metrics import accuracy_score, precision_score, recall_score
pd.set_option('mode.chained_assignment', None)

# Number of bootstrap iterations
n_rounds = 200

# Separate data test from data
data_in = pd.read_csv('GT.csv')
data_in['label'] = data_in['label'].astype(str)

test_inputs = data_in['label'].str.contains('T')
 
data = data_in.loc[~test_inputs]
data_test = data_in.loc[test_inputs]

if not data_test.empty:
	data_test_X = data_test.drop('label', axis=1)
	data_test_y = data_test['label'].str.replace('T', '')
	size_data_test0 = len(data_test.loc[data['label'] == '0'])
	size_data_test1 = len(data_test.loc[data['label'] == '1'])
	print("Data test present: label0=", size_data_test0, 
		                     "label1=", size_data_test1)

# Print pool size
size_train_label0 = len(data.loc[data['label'] == '0'])
size_train_label1 = len(data.loc[data['label'] == '1'])
print("train samples: label0=", size_train_label0,
      "label1 = ", size_train_label1);


# Instantiates classifiers
classifiers = list();   # list of classifiers
def add(clf, name):
	classifiers.append(clf)
	clf.name = name;       # name of a classifier
	clf.scores = list();   # list of scores within a classifier

# We use default params' except for classifier size
add( KNeighborsClassifier(n_neighbors=3), "KN(3)");
add( KNeighborsClassifier(n_neighbors=5), "KN(5)");
add( RandomForestClassifier(n_estimators=20), "RF(n=20)")
add( RandomForestClassifier(n_estimators=10), "RF(n=10)")
add( RandomForestClassifier(n_estimators=5),  "RF(n=5)")
add( RandomForestClassifier(n_estimators=2),  "RF(n=2)")
add( RandomForestClassifier(n_estimators=10, max_leaf_nodes=50),  "RF(n=2,m=50)")
add( RandomForestClassifier(n_estimators=10, max_leaf_nodes=20),  "RF(n=2,m=20)")
add( svm.SVC(), "SVC" )
add( MLPClassifier(hidden_layer_sizes=[12], max_iter=1000), "NN(12)" )
add( MLPClassifier(hidden_layer_sizes=[6,6], max_iter=1000), "NN(6,6)" )
add( MLPClassifier(hidden_layer_sizes=[4,4,4], max_iter=1000), "NN(4,4,4)" )
add( MLPClassifier(hidden_layer_sizes=[24], max_iter=1000), "NN(24)" )
add( MLPClassifier(hidden_layer_sizes=[12,12], max_iter=1000), "NN(12,12)" )
add( MLPClassifier(hidden_layer_sizes=[8,8,8], max_iter=1000), "NN(8,8,8)" )


# Training & testing
X = data.drop('label', axis=1)
y = data['label']
for i in range(n_rounds):
	Xtr, Xt, ytr, yt = train_test_split(X, y, test_size=0.2, random_state=i)
	for el in classifiers:
		print(el.name, " ", end='',flush=True, file=sys.stderr);
		el.fit(Xtr, ytr)
		if not data_test.empty:
			Xt = data_test_X
			yt = data_test_y
		pred = el.predict(Xt) 

		# calculate and append scores to the list
		el.scores.append( (accuracy_score(yt, pred), "accuracy") )
		el.scores.append( (precision_score(yt, pred, pos_label='0'), "prec0") )
		el.scores.append( (precision_score(yt, pred, pos_label='1'), "prec1") )
		el.scores.append( (recall_score(yt, pred, pos_label='0'), "rec0") )
		el.scores.append( (recall_score(yt, pred, pos_label='1'), "rec1") )
	print(file=sys.stderr);


# Print results
score_labels = set([x[1] for x in classifiers[0].scores])
for clf in classifiers:
	for score in score_labels:
		a = pd.Series([x[0] for x in clf.scores if x[1]==score])
		print("%-15s" % clf.name, "%-10s" % score, end='')
		qnt = tuple(a.quantile([0.05, 0.25, 0.5, 0.75, 0.95]))
		print("%8.4lf %8.4lf %8.4lf %8.4lf %8.4lf" % qnt);


