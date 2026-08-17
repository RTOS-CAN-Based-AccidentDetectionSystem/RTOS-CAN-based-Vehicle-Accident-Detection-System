import mysql.connector


def get_db_connection():

    connection = mysql.connector.connect(
        host="localhost",
        user="accident_user",
        password="accident123",
        database="Accident_Detection_System"
    )

    return connection