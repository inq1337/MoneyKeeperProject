from flask import Flask, request
from random import randint
import json, os


abspath = os.path.abspath(__file__)
dirname = os.path.dirname(abspath)
os.chdir(dirname)

users_base = list()

def read_json():
    global users_base
    if os.path.isfile('usersBase.json'):
        with open('usersBase.json', 'r') as read_file:
            users_base = json.load(read_file)
            read_file.close()


def write_to_json():
    global users_base
    with open('usersBase.json', 'w') as write_file:
        json.dump(users_base, write_file, indent=2)
        write_file.close()


def add_user(login, password: str, user_id: int):
    global users_base
    users_base += [{"id": user_id, "login": login, "password": password, "data": list(), "plans": list()}]
    write_to_json()


def add_cost(user_id, cost_id, cost_sum: int, cost_date, cost_category, cost_comment: str):
    global users_base
    for user in users_base:
        if user["id"] == user_id:
            user["data"] += [{"category": cost_category, "comment": cost_comment, "date": cost_date, "id": cost_id, "sum": cost_sum}]
            write_to_json()
            return 'OK'
            break
    else:
        return 'no such user'


def get_all(user_id: int, password: str):
    for user in users_base:
        if user["id"] == user_id:
            if user["password"] == password:
                return {"data": user["data"], "plans": user["plans"]}
                break
            else:
                return 'wrong password'
    else:
        return 'no such user'


def remove_cost(user_id, cost_id: int):
    global users_base
    for user in users_base:
        if user["id"] == user_id:
            for cost in user['data']:
                if cost['id'] == cost_id:
                    user['data'].remove(cost)
                    write_to_json()
                    return 'OK'
                    break
            else:
                return 'no such cost' 
            break
    else:
        return 'no such user'        


def add_plan(user_id, plan_category, plan_sum):
    global users_base
    for user in users_base:
        if user["id"] == user_id:
            user["plans"] += [{"category": plan_category, "sum": plan_sum}]
            write_to_json()
            return 'OK'
            break
    else:
        return 'no such user'

def remove_plan(user_id, plan_category):
    global users_base
    for user in users_base:
        if user["id"] == user_id:
            for plan in user['plans']:
                if plan['category'] == plan_category:
                    user['plans'].remove(plan)
                    write_to_json()
                    return 'OK'
                    break
            else:
                return 'no such cost' 
            break
    else:
        return 'no such user'      


read_json()
app = Flask(__name__)

@app.route("/register", methods=['GET'])
def register_f():
    login = request.args.get('login')
    password = request.args.get('password')
    existence = True
    while existence:
        user_id = randint(1, 10000)
        for i in range(len(users_base)):
            if users_base[i]["id"] == user_id:
                break
        else:
            existence = False
                
    for i in range(len(users_base)):
        if users_base[i]["login"] == login:
            return 'alreadyExists'

    add_user(login, password, user_id)
    
    return str(user_id)


@app.route("/login", methods=['GET'])
def login_f():
    login = request.args.get('login')
    password = request.args.get('password')
                
    for i in range(len(users_base)):
        if users_base[i]['login'] == login:
            if users_base[i]['password'] == password:
                return str(users_base[i]['id'])
            else:
                return 'wrong password'
            
    return 'no such user'


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
    password = request.args.get('password')

    return get_all(user_id, password)


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

