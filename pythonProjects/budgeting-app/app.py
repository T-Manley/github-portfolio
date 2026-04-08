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
    month = request.args.get("month")

    conn = sqlite3.connect("expenses.db")
    cursor = conn.cursor()

    if month:
        cursor.execute("""
            SELECT id, name, amount, category, date
            FROM expenses
            WHERE strftime('%Y-%m', date) = ?
        """, (month,))
    
    else:
        cursor.execute("SELECT id, name, amount, category, date FROM expenses")

    rows = cursor.fetchall()
    conn.close()

    expenses = [
        {
            "id": row[0],
            "name": row[1],
            "amount": row[2],
            "category": row[3],
            "date": row[4]
        }
        for row in rows
    ]
    
    return jsonify(expenses)

@app.route("/total", methods=["GET"])
def get_total():
    month = request.args.get("month")

    conn = sqlite3.connect("expenses.db")
    cursor = conn.cursor()
    
    if month:
        cursor.execute("""
            SELECT SUM(amount) FROM expenses
            WHERE strftime('%Y-%m', date) = ?
        """, (month,))
    else:
        cursor.execute("SELECT SUM(amount) FROM expenses")

    total = cursor.fetchone()[0]
    conn.close()
    
    return jsonify({"total": total if total else 0})

@app.route("/delete/<int:expense_id>", methods=["DELETE"])
def delete_expense(expense_id):
    conn = sqlite3.connect("expenses.db")
    cursor = conn.cursor()
    cursor.execute("DELETE FROM expenses WHERE id = ?", (expense_id,))
    conn.commit()
    conn.close()

    return jsonify({"message": "Expense deleted"})





if __name__ == "__main__":
    app.run(debug=True)