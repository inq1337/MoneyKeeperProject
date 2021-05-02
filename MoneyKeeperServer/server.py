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
            break
    write_to_json()


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
    print(users_base)
    existence = False
    for user in users_base:
        print(type(user['id']))
        if user["id"] == user_id:
            print(1)
            existence = True
            for cost in user['data']:
                print(cost)
                print(type(cost))
                if cost['id'] == cost_id:
                    print(2)
                    user['data'].remove(cost)
                    write_to_json()
                    break
            break
    if existence:
        return 'OK'
    else:
        return 'no such cost'            


read_json()
app = Flask(__name__)

@app.route("/register", methods=['GET'])
def login_f():
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
def register_f():
    login = request.args.get('login')
    password = request.args.get('password')
                
    for i in range(len(users_base)):
        if users_base[i]["login"] == login:
            return str(users_base[i]['id'])
            
    return 'noneUser'


@app.route("/addCosts", methods=['GET'])
def add_cost_f():
    user_id = int(request.args.get('userID'))
    cost_id = int(request.args.get('id'))
    cost_date = request.args.get('date')
    cost_category = request.args.get('category')
    cost_sum = int(request.args.get('sum'))
    cost_comment = request.args.get('comment')
    print(user_id, cost_id, cost_sum, cost_date, cost_category, cost_comment)
    add_cost(user_id, cost_id, cost_sum, cost_date, cost_category, cost_comment)

    return 'OK'


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

if __name__ == "__main__":
    app.run()

