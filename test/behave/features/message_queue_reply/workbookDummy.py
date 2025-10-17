from flask import Flask, request, jsonify

app = Flask(__name__)


@app.route("/login", methods=["POST"])
def hello_world():
    data = request.get_json()
    if data["username"] == "scr" and data["password"] == "scr":

        return jsonify([{"status": "OK"}]), 200

    return jsonify([{"status": "NOTOK"}]), 403
