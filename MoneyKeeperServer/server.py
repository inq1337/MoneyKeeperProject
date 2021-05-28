from flask import Flask, request
from os import path, chdir
import sqlite3

abspath = path.abspath(__file__)
dirname = path.dirname(abspath)
chdir(dirname)

max_possible_user_id = 10000

conn = sqlite3.connect('usersBase.db', check_same_thread = False)
cur = conn.cursor()

cur.execute("""CREATE TABLE IF NOT EXISTS users(
   user_id INT PRIMARY KEY,
   login TEXT,
   password TEXT);
""")
cur = conn.cursor()
cur.execute("""CREATE TABLE IF NOT EXISTS costs(
   cost_id INT,
   date TEXT,
   category TEXT,
   sum INT,
   comment TEXT);
""")
cur = conn.cursor()
cur.execute("""CREATE TABLE IF NOT EXISTS plans(
   plan_id INT,
   category TEXT,
   sum INT);
""")


max_user_id = 0
cur.execute("SELECT user_id FROM users;")
all_results = cur.fetchall()
for user in all_results:
    if user[0] > max_user_id:
        max_user_id = user[0]


def optimize_costs_id():
    cur.execute("SELECT * FROM costs;")
    costs_arr = cur.fetchall()
    for i in range(len(costs_arr)):
        user_id = costs_arr[i][0] // max_possible_user_id
        curr_id = user_id * max_possible_user_id + i + 1
        item = costs_arr[i]
        new_item = (curr_id, item[1], item[2], item[3], item[4])
        costs_arr[i] = new_item
    cur.execute("DELETE FROM costs;")
    cur.executemany("INSERT INTO costs VALUES(?, ?, ?, ?, ?);", costs_arr)


def optimize_plans_id():
    cur.execute("SELECT * FROM plans;")
    plans_arr = cur.fetchall()
    for i in range(len(plans_arr)):
        user_id = plans_arr[i][0] // max_possible_user_id
        curr_id = user_id * max_possible_user_id + i + 1
        item = plans_arr[i]
        new_item = (curr_id, item[1], item[2])
        plans_arr[i] = new_item
    cur.execute("DELETE FROM plans;")
    cur.executemany("INSERT INTO plans VALUES(?, ?, ?);", plans_arr)


def check_reg_possibility(user_id: int) -> bool:
    global max_possible_user_id
    if user_id < max_possible_user_id:
        return True
    else:
        return False


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
        if check_reg_possibility(user_id):
            max_user_id += 1

            optimize_plans_id()
            optimize_costs_id()
            user = (user_id, login, password)
            cur.execute("INSERT INTO users VALUES(?, ?, ?);", user)
            conn.commit()

            return str(user_id)
        else:
            return 'maximum reached'


def add_cost(user_id, cost_id, sum: int, date, category, comment: str):
    if user_id_existence(user_id):
        cost = (cost_id, date, category, sum, comment)
        cur.execute("INSERT INTO costs VALUES(?, ?, ?, ?, ?);", cost)
        conn.commit()
        return 'OK'
    else:
        return 'no such user'


def get_all(user_id: int):
    if user_id_existence(user_id):
        data_range_min = user_id * max_possible_user_id + 1
        data_range_max = (user_id + 1) * max_possible_user_id
        user_costs_arr = []

        for cost_id in range(data_range_min, data_range_max):
            cur.execute("SELECT * FROM costs WHERE cost_id = ?", (cost_id,))
            one_res = cur.fetchone()
            if one_res is None:
                continue
            else:
                user_costs_arr.append(one_res)

        costs = []
        for cost in user_costs_arr:
            costs.append({"id": cost[0], "date": cost[1], "category": cost[2], "sum": cost[3], "comment": cost[4]})
            
        user_plans_arr = []
        for plan_id in range(data_range_min, data_range_max):
            cur.execute("SELECT * FROM plans WHERE plan_id = ?", (plan_id,))
            one_res = cur.fetchone()
            if one_res is None:
                continue
            else:
                user_plans_arr.append(one_res)
        
        plans = []
        for plan in user_plans_arr:
            plans.append({"id": plan[0], "category": plan[1], "sum": plan[2]})
        
        print({"data": costs, "plans": plans})
        return {"data": costs, "plans": plans}
    else:
        return 'no such user'


def remove_cost(user_id, cost_id: int):
    if user_id_existence(user_id):
        cur.execute("DELETE FROM costs WHERE cost_id = ?;", (cost_id,))
        conn.commit()

        return 'OK'
    else:
        return 'no such user'


def add_plan(user_id, plan_id, plan_category, sum):
    if user_id_existence(user_id):
        plan = (plan_id, plan_category, sum)
        cur.execute("INSERT INTO plans VALUES(?, ?, ?);", plan)
        conn.commit()

        return 'OK'
    else:
        return 'no such user'

def remove_plan(user_id, plan_id):
    if user_id_existence(user_id):
        print(plan_id)
        cur.execute("DELETE FROM plans WHERE plan_id = ?;", (plan_id,))
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
    plan_id = request.args.get('planID')
    plan_category = request.args.get('category')
    plan_sum = int(request.args.get('sum'))

    return add_plan(user_id, plan_id, plan_category, plan_sum)


@app.route("/removePlan", methods=['GET'])
def remove_plan_f():
    user_id = int(request.args.get('userID'))
    plan_id = request.args.get('planID')

    return remove_plan(user_id, plan_id)

if __name__ == "__main__":
    app.run()
