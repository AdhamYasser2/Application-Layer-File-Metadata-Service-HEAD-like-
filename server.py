from flask import Flask, jsonify, send_file
from flask_cors import CORS
import os
import hashlib

app = Flask(__name__)
CORS(app)

FILE_PATH = "data.txt"


def calculate_file_hash(file_path):
    """
    Calculate MD5 hash for the file.
    If the file content changes, the hash will change.
    """
    with open(file_path, "rb") as file: # rb -> read binary 
        file_data = file.read()
        return hashlib.md5(file_data).hexdigest()


@app.route("/metadata", methods=["GET"])
def get_metadata():
    """
    HEAD-like request:
    Arduino asks for file metadata only, not full content.
    """
    if not os.path.exists(FILE_PATH):
        return jsonify({
            "error": "File not found"
        }), 404

    file_size = os.path.getsize(FILE_PATH)
    file_hash = calculate_file_hash(FILE_PATH)

    return jsonify({
        "filename": FILE_PATH,
        "size": file_size,
        "hash": file_hash
    })


@app.route("/file", methods=["GET"])
def get_file():
    """
    Arduino requests the full file only if hash changed.
    """
    if not os.path.exists(FILE_PATH):
        return jsonify({
            "error": "File not found"
        }), 404

    return send_file(FILE_PATH, as_attachment=False)


@app.route("/", methods=["GET"])
def home():
    return "Python Metadata Server is running!"


if __name__ == "__main__":
    app.run(host="0.0.0.0" , port=8000, debug=True)
