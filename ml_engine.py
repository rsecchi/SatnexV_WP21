#!/usr/local/bin/python3

import pandas as pd
from sklearn.model_selection import train_test_split
from sklearn.ensemble import RandomForestClassifier
from sklearn.metrics import confusion_matrix, classification_report

data = pd.read_csv('GT.csv');

data.info() 
X = data.drop('label', axis=1)
y = data['label']

X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

rfc = RandomForestClassifier(n_estimators=200)
rfc.fit(X_train, y_train)

pred_rfc = rfc.predict(X_test)

print(classification_report(y_test, pred_rfc))

