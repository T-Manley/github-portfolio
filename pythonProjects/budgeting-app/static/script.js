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

    // refresh the expense list, total and delete
    loadExpenses(); 
    loadTotal();
});

async function loadExpenses(month = null) {
    let url = "/expenses";

    if (month) {
        url += `?month=${month}`;
    }
    
    const response = await fetch(url);
    const data = await response.json();

    const list = document.getElementById("expense-list");
    list.innerHTML = ""; // clear existing items

    const categoryTotals = {};

    data.forEach(expense => {
        const li = document.createElement("li");
        li.textContent = `${expense.name}: - $${expense.amount.toFixed(2)} (${expense.category} on ${expense.date})`;

        const deleteBtn = document.createElement("button");
        deleteBtn.textContent = "Delete";
        deleteBtn.style.marginLeft = "10px";

        deleteBtn.onclick = async function() {
            await fetch(`/delete/${expense.id}`, { method: "DELETE" });

            const selectedMonth = document.getElementById("month-filter").value;
            loadExpenses(selectedMonth);
            loadTotal(selectedMonth);
        };

        li.appendChild(deleteBtn);
        list.appendChild(li);

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

    renderCategoryChart(categoryTotals);
}

let categoryChart = null;

function renderCategoryChart(categoryTotals) {
    const ctx = document.getElementById("category-chart").getContext("2d");

    const labels = Object.keys(categoryTotals);
    const data = Object.values(categoryTotals);

    if (categoryChart) {
        categoryChart.destroy();
    }

    categoryChart = new Chart(ctx, {
        type: "doughnut",
        data: {
            labels: labels,
            datasets: [{
                data: data,
                backgroundColor: [
                    "#FF6384",
                    "#36A2EB",
                    "#FFCE56",
                    "#4BC0C0",
                    "#9966FF",
                    "#FF9F40"
                ],
                borderWidth: 2,
                borderColor: "#000000",
            }]
        },
        options: {
            cutout: "70%",
            responsive: true,
            animation: {
                animateRotate: true,
                animateScale: true
            },
            plugins: {
                legend: {
                    position: "bottom"
                },
                tooltip: {
                    callbacks: {
                        label: function(context) {
                            return `${context.label}: $${context.parsed.toFixed(2)}`;
                        }
                    }
                }
            }
        }
    });
}

function loadTotal(month = null) {
    let url = "/total";

    if (month) {
        url += `?month=${month}`;
    }

    fetch(url)
        .then(res => res.json())
        .then(data => {
            document.getElementById("total-amount").textContent = data.total.toFixed(2);
        });
}

document.getElementById("month-filter").addEventListener("change", function() {
    const selectedMonth = this.value;
    loadExpenses(selectedMonth);
    loadTotal(selectedMonth);
});

document.getElementById("clear-filter").addEventListener("click", function() {
    document.getElementById("month-filter").value = "";
    loadExpenses();
    loadTotal();
});


// load on page load
loadExpenses(); 
loadTotal();
