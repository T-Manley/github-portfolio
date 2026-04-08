function sayHello() {
    alert("Hello from JavaScript!");
}

document.getElementById("expense-form").addEventListener("submit", async function(event) {
    event.preventDefault(); // stop page reload

    const expenseData = {
        name: document.getElementById("name").value,
        amount: parseFloat(document.getElementById("amount").value),
        category: document.getElementById("category").value,
        date: document.getElementById("date").value
    };

    const response = await fetch("/add", {
        method: "POST",
        headers: {
            "Content-Type": "application/json"
        },
        body: JSON.stringify(expenseData)
    });

    const result = await response.json();
    alert(result.message);
    loadExpenses(); // refresh the expense list
});

async function loadExpenses() {
    const response = await fetch("/expenses");
    const data = await response.json();

    const list = document.getElementById("expense-list");
    const totalDisplay = document.getElementById("total");

    list.innerHTML = ""; // clear existing items

    let total = 0;

    data.forEach(expense => {
        const li = document.createElement("li");
        li.textContent = `${expense.name}: - $${expense.amount.toFixed(2)} (${expense.category} on ${expense.date})`;
        list.appendChild(li);

        total += expense.amount;
    });

    totalDisplay.textContent = total.toFixed(2);

    const categoryTotals = {};

    data.forEach(expense => {
        if (!categoryTotals[expense.category]) {
            categoryTotals[expense.category] = 0;
        }
        categoryTotals[expense.category] += expense.amount;
    });

    const breakdownList = document.getElementById("category-breakdown");
    breakdownList.innerHTML = ""; // clear existing items

    for (const category in categoryTotals) {
        const li = document.createElement("li");
        li.textContent = `${category}: $${categoryTotals[category].toFixed(2)}`;
        breakdownList.appendChild(li);
    }
}

loadExpenses(); // load expenses on page load