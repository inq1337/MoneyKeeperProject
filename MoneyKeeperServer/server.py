from flask import Flask, request
from os import path, chdir
import sqlite3

abspath = path.abspath(__file__)
dirname = path.dirname(abspath)
chdir(dirname)

conn = sqlite3.connect('usersBase.db', check_same_thread = False)
cur = conn.cursor()

cur.execute("""CREATE TABLE IF NOT EXISTS users(
   user_id INT PRIMARY KEY,
   login TEXT,
   password TEXT);
""")
cur = conn.cursor()
cur.execute("""CREATE TABLE IF NOT EXISTS costs(
   user_id INT,
   cost_id INT,
   date TEXT,
   category TEXT,
   sum INT,
   comment TEXT);
""")
cur = conn.cursor()
cur.execute("""CREATE TABLE IF NOT EXISTS plans(
   user_id,
   category TEXT,
   sum INT);
""")


max_user_id = 0
cur.execute("SELECT user_id FROM users;")
all_results = cur.fetchall()
for user in all_results:
    if user[0] > max_user_id:
        max_user_id = user[0]


def user_login_existence(login: str) -> bool:
    cur.execute("SELECT * FROM users WHERE login = ?", (login,))
    data=cur.fetchone()
    if data is None:
        return False
    else:
        return True


def user_id_existence(user_id: int) -> bool:
    cur.execute("SELECT * FROM users WHERE user_id = ?", (user_id,))
    data=cur.fetchone()
    if data is None:
        return False
    else:
        return True


def authorization(login, password: str):
    if not user_login_existence(login):
        return 'no such user'
    else:
        cur.execute("SELECT * FROM users WHERE login = ?", (login,))
        user = cur.fetchone()
        if user[2] == password:
            return str(user[0])
        else:
            return 'wrong password'
            


def add_user(login, password: str):
    if user_login_existence(login):
            return 'already exists'
    else:
        global max_user_id
        user_id = max_user_id + 1
        max_user_id += 1
        user = (user_id, login, password)
        cur.execute("INSERT INTO users VALUES(?, ?, ?);", user)
        conn.commit()

        return str(user_id)


def add_cost(user_id, cost_id, sum: int, date, category, comment: str):
    if user_id_existence(user_id):
        cost = (user_id, cost_id, date, category, sum, comment)
        cur.execute("INSERT INTO costs VALUES(?, ?, ?, ?, ?, ?);", cost)
        conn.commit()
        return 'OK'
    else:
        return 'no such user'


def get_all(user_id: int):
    if user_id_existence(user_id):
        cur.execute("SELECT * FROM costs WHERE user_id = ?", (user_id,))
        user_costs_raw = cur.fetchall()
        costs = []
        for cost in user_costs_raw:
            costs.append({"id": cost[1], "date": cost[2], "category": cost[3], "sum": cost[4], "comment": cost[5]})
        
        cur.execute("SELECT * FROM plans WHERE user_id = ?", (user_id,))
        user_plans_raw = cur.fetchall()
        plans = []
        for plan in user_plans_raw:
            plans.append({"category": plan[1], "sum": plan[2]})
        
        return {"data": costs, "plans": plans}
    else:
        return 'no such user'


def remove_cost(user_id, cost_id: int):
    if user_id_existence(user_id):
        cur.execute("DELETE FROM costs WHERE user_id = ? and cost_id = ?;", (user_id, cost_id))
        conn.commit()

        return 'OK'
    else:
        return 'no such user'        


def add_plan(user_id, category, sum):
    if user_id_existence(user_id):
        plan = (user_id, category, sum)
        cur.execute("INSERT INTO plans VALUES(?, ?, ?);", plan)
        conn.commit()

        return 'OK'
    else:
        return 'no such user'

def remove_plan(user_id, category):
    if user_id_existence(user_id):
        cur.execute("DELETE FROM plans WHERE user_id = ? and category = ?;", (user_id, category))
        conn.commit()

        return 'OK'
    else:
        return 'no such user'

app = Flask(__name__)

@app.route("/register", methods=['GET'])
def register_f():
    login = request.args.get('login')
    password = request.args.get('password')
    
    return add_user(login, password)


@app.route("/login", methods=['GET'])
def login_f():
    login = request.args.get('login')
    password = request.args.get('password')
    
    return authorization(login, password)


@app.route("/addCosts", methods=['GET'])
def add_cost_f():
    user_id = int(request.args.get('userID'))
    cost_id = int(request.args.get('id'))
    cost_date = request.args.get('date')
    cost_category = request.args.get('category')
    cost_sum = int(request.args.get('sum'))
    cost_comment = request.args.get('comment')

    return add_cost(user_id, cost_id, cost_sum, cost_date, cost_category, cost_comment)


@app.route("/getAll", methods=['GET'])
def get_all_f():
    user_id = int(request.args.get('userID'))

    return get_all(user_id)


@app.route("/removeCost", methods=['GET'])
def remove_cost_f():
    user_id = int(request.args.get('userID'))
    cost_id = int(request.args.get('costID'))

    return remove_cost(user_id, cost_id)


@app.route("/addPlan", methods=['GET'])
def add_plan_f():
    user_id = int(request.args.get('userID'))
    plan_category = request.args.get('category')
    plan_sum = int(request.args.get('sum'))

    return add_plan(user_id, plan_category, plan_sum)


@app.route("/removePlan", methods=['GET'])
def remove_plan_f():
    user_id = int(request.args.get('userID'))
    plan_category = request.args.get('category')

    return remove_plan(user_id, plan_category)

if __name__ == "__main__":
    app.run()
