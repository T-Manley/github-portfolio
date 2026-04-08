from flask import Flask, render_template, request, jsonify
import sqlite3

app = Flask(__name__)

# Initialize the database
def init_db():
    conn = sqlite3.connect("expenses.db")
    cursor = conn.cursor()
    cursor.execute("""
        CREATE TABLE IF NOT EXISTS expenses (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT,
            amount REAL,
            category TEXT,
            date TEXT
        )
    """)
    conn.commit()
    conn.close()

init_db()

@app.route("/")
def home():
    name = "User"
    return render_template("index.html", username=name)

expenses = []

@app.route("/add", methods=["POST"])
def add_expense():
    data = request.get_json()
    
    conn = sqlite3.connect("expenses.db")
    cursor = conn.cursor()
    cursor.execute(
        "INSERT INTO expenses (name, amount, category, date) VALUES (?, ?, ?, ?)",
        (data["name"], data["amount"], data["category"], data["date"])
    )
    conn.commit()
    conn.close()

    return jsonify({"message": "Expense added successfully!"})

@app.route("/expenses", methods=["GET"])
def get_expenses():
    conn = sqlite3.connect("expenses.db")
    cursor = conn.cursor()
    cursor.execute("SELECT name, amount, category, date FROM expenses")
    rows = cursor.fetchall()
    conn.close()

    expenses = [
        {"name": row[0], "amount": row[1], "category": row[2], "date": row[3]}
        for row in rows
    ]
    
    return jsonify(expenses)

if __name__ == "__main__":
    app.run(debug=True)