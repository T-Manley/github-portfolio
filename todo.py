import datetime
import os
import pickle

class Task:
    def __init__(self, name, due_date):
        self.name = name
        self.due_date = due_date
        self.completed = False

    def change_name(self, new_name):
        self.name = new_name

    def change_due_date(self, new_date):
        self.due_date = new_date

    def complete(self):
        self.completed = True

tasks = []

menu = {
    1: "Add New Task",
    2: "View All Tasks",
    3: "Mark Task as Completed",
    4: "Delete Task",
    5: "Save Tasks",
    6: "Load Tasks",
    7: "Exit"
}

print("To-Do List Menu")
for key, value in menu.items():
    print(f"{key} -- {value}")

choice = input('Please enter your selection: ')

def add_task():
    name = input("Enter task name: ")
    due_date = input("Enter due date: ")
    new_task = Task(name, due_date)
    tasks.append(new_task)

def view_tasks():
    print('All Tasks: ')

    for task in tasks:
        print(f'Name: {task.name}')
        print(f'Due Date: {task.due_date}')
        print(f'Completed: {task.completed}')
        print()

def complete_task():
    index = int(input("Enter the task number to mark as completed: "))
    tasks[index - 1].complete()

def delete_task():
    index = int(input("Enter the task number to delete: "))
    tasks.pop(index - 1)

def save_tasks():
    file = open('tasks.dat', 'wb')

    pickle.dump(tasks, file)

    file.close()

def load_tasks():
    global tasks

    file = open('tasks.dat', 'rb')
    tasks = pickle.load(file)

    file.close()

def display_menu():
    print("To-Do List Menu")
    for key, value in menu.items():
        print(f"{key} -- {value}")

while choice != '7':

    if choice == '1':
        add_task()
    elif choice == '2':
        view_tasks()
    elif choice == '3':
        complete_task()
    elif choice == '4':
        delete_task()
    elif choice == '5':
        save_tasks()
    elif choice == '6':
        load_tasks()
    else:
        print("Invalid selection. Please try again.")
    
    input('Press enter to return to menu: ')
    display_menu()
    choice = input('Please enter your selection: ')