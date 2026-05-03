# Support Triage Agent

## 📌 Overview

This project is a **terminal-based support triage agent** that processes customer support tickets and decides whether to **reply automatically** or **escalate to a human agent**.

It simulates how real-world support systems work by combining:

* Issue understanding
* Risk detection
* Decision making
* Structured output generation


## ⚙️ Features

* 🔍 **Request Classification**

  * Categorizes issues into:

    * `bug`
    * `feature_request`
    * `product_issue`

* 🧠 **Product Area Detection**

  * Identifies domains such as:

    * Payments
    * Fraud & Security
    * Account Access
    * API
    * Assessments

* ⚠️ **Risk Detection & Escalation**

  * Detects sensitive issues like:

    * Fraud
    * Unauthorized access
    * Payment issues
  * Automatically escalates high-risk or uncertain cases

* 📊 **Confidence-Based Decision Making**

  * Uses keyword-based confidence scoring
  * Low confidence → escalated (safety-first approach)

* ✍️ **Response Generation**

  * Generates context-aware responses for safe cases

* 🧾 **Justification Output**

  * Explains why each decision was made

* 📁 **CSV Input/Output**

  * Reads support tickets from `support_issues.csv`
  * Writes structured results to `output.csv`

---

## 📂 Input Format (`support_issues.csv`)

```
issue,company
Payment failed twice,Visa
Login not working,HackerRank
API error,Claude
```

---

## 📤 Output Format (`output.csv`)

```
status,product_area,response,justification,request_type
escalated,Payments,"...","...",bug
replied,Account Access,"...","...",product_issue
```

---

## ▶️ How to Run

### 1. Compile

```
g++ main.cpp -o main
```

### 2. Run

```
./main
```

### 3. Output

* Results will be saved in `output.csv`

---

## 🧠 Decision Logic

The agent follows this pipeline:

1. **Read issue from CSV**
2. **Normalize text (lowercase)**
3. **Classify request type**
4. **Identify product area**
5. **Detect risk keywords**
6. **Calculate confidence score**
7. **Decide: reply or escalate**
8. **Generate response + justification**
9. **Write output to CSV**

---

## 🚨 Escalation Rules

The system escalates when:

* Fraud / unauthorized access is detected
* Payment-related risks are present
* Confidence score is low
* Issue cannot be clearly classified (General category)

---

## 🛠️ Tech Stack

* Language: **C++**
* Concepts:

  * String processing
  * Rule-based classification
  * File handling (CSV)
  * Decision systems

---

## 🚀 Future Improvements

* Better text matching (TF-IDF / embeddings)
* Multi-intent detection
* Advanced confidence scoring
* Integration with real support APIs

---

## 👨‍💻 Author

Piyush Mishra

---

## 🏁 Summary

This project demonstrates how to build a **safe, explainable, rule-based support system** that balances automation with human escalation.
