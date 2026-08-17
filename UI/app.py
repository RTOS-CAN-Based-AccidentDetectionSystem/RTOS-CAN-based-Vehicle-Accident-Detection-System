from flask import Flask, jsonify, request, render_template
from flask_socketio import SocketIO
from db import get_db_connection
from datetime import datetime

app = Flask(__name__)

# ============================================================
# FLASK-SOCKETIO
# ============================================================

socketio = SocketIO(
    app,
    cors_allowed_origins="*",
    async_mode="threading"
)

# ============================================================
# HOME API
# ============================================================

@app.route("/")
def home():

    return "Accident Detection API is running"


# ============================================================
# DASHBOARD
# ============================================================

@app.route("/dashboard")
def dashboard():

    return render_template("dashboard.html")


# ============================================================
# ADD VEHICLE
# POST /api/vehicle
# ============================================================

@app.route("/api/vehicle", methods=["POST"])
def add_vehicle():

    connection = None
    cursor = None

    try:

        # ----------------------------------------------------
        # Get JSON data
        # ----------------------------------------------------

        data = request.get_json()

        if not data:
            return jsonify({
                "error": "No JSON data received"
            }), 400


        vehicle_no = data["vehicle_no"]
        vehicle_type = data["vehicle_type"]
        owner = data["owner"]
        phone = data["phone"]


        # ----------------------------------------------------
        # Connect to MySQL
        # ----------------------------------------------------

        connection = get_db_connection()

        cursor = connection.cursor()


        # ----------------------------------------------------
        # Insert vehicle
        # ----------------------------------------------------

        query = """
            INSERT INTO vehicle_info
            (
                Vehicle_No,
                Vehicle_Type,
                Owner,
                Phone
            )
            VALUES (%s, %s, %s, %s)
        """


        values = (
            vehicle_no,
            vehicle_type,
            owner,
            phone
        )


        cursor.execute(
            query,
            values
        )


        connection.commit()


        return jsonify({

            "message": "Vehicle added successfully",

            "vehicle_no": vehicle_no

        }), 201


    except Exception as e:

        if connection:
            connection.rollback()

        return jsonify({

            "error": str(e)

        }), 500


    finally:

        if cursor:
            cursor.close()

        if connection:
            connection.close()


# ============================================================
# GET ALL VEHICLES
# GET /api/vehicles
# ============================================================

@app.route("/api/vehicles", methods=["GET"])
def get_vehicles():

    connection = None
    cursor = None

    try:

        # ----------------------------------------------------
        # Connect to MySQL
        # ----------------------------------------------------

        connection = get_db_connection()


        # Dictionary cursor gives column names
        cursor = connection.cursor(
            dictionary=True
        )


        # ----------------------------------------------------
        # Get all vehicles
        # ----------------------------------------------------

        cursor.execute(
            """
            SELECT *
            FROM vehicle_info
            ORDER BY Vehicle_No
            """
        )


        vehicles = cursor.fetchall()


        return jsonify(vehicles), 200


    except Exception as e:

        return jsonify({

            "error": str(e)

        }), 500


    finally:

        if cursor:
            cursor.close()

        if connection:
            connection.close()


# ============================================================
# ADD ACCIDENT
# POST /api/accident
# ============================================================

@app.route("/api/accident", methods=["POST"])
def add_accident():

    connection = None
    cursor = None

    try:

        # ----------------------------------------------------
        # Get JSON data
        # ----------------------------------------------------

        data = request.get_json()

        if not data:

            return jsonify({

                "error": "No JSON data received"

            }), 400


        # ----------------------------------------------------
        # Extract accident data
        # ----------------------------------------------------

        vehicle_no = data["vehicle_no"]

        latitude = data["latitude"]

        longitude = data["longitude"]


        # ----------------------------------------------------
        # Connect to MySQL
        # ----------------------------------------------------

        connection = get_db_connection()

        cursor = connection.cursor(
            dictionary=True
        )


        # ----------------------------------------------------
        # Check whether vehicle exists
        # ----------------------------------------------------

        cursor.execute(

            """
            SELECT
                Vehicle_No,
                Vehicle_Type,
                Owner,
                Phone

            FROM vehicle_info

            WHERE Vehicle_No = %s
            """,

            (vehicle_no,)

        )


        vehicle = cursor.fetchone()


        # ----------------------------------------------------
        # If vehicle does not exist
        # ----------------------------------------------------

        if not vehicle:

            return jsonify({

                "error": "Vehicle not registered"

            }), 404


        # ----------------------------------------------------
        # Insert accident
        # ----------------------------------------------------

        cursor.execute(

            """
            INSERT INTO accident_info
            (
                Vehicle_No,
                Latitude,
                Longitude
            )

            VALUES
            (
                %s,
                %s,
                %s
            )
            """,

            (
                vehicle_no,
                latitude,
                longitude
            )

        )


        connection.commit()


        # ----------------------------------------------------
        # Get Accident ID
        # ----------------------------------------------------

        accident_id = cursor.lastrowid


        # ----------------------------------------------------
        # Prepare real-time accident data
        # ----------------------------------------------------

        accident_data = {

            "accident_id": accident_id,

            "vehicle_no": vehicle_no,

            "vehicle_type": vehicle["Vehicle_Type"],

            "owner": vehicle["Owner"],

            "phone": vehicle["Phone"],

            "latitude": float(latitude),

            "longitude": float(longitude)

        }


        # ----------------------------------------------------
        # Send real-time event to dashboard
        # ----------------------------------------------------

        socketio.emit(

            "new_accident",

            {

                "type": "ACCIDENT",

                "message": "New accident detected",

                "data": accident_data

            }

        )

        # ----------------------------------------------------
        # Return API response
        # ----------------------------------------------------

        return jsonify({

            "success":True,

            "message":"Accident stored successfully",

            "dashboard_updated":True,

            "data":accident_data

        }),201

    except Exception as e:

        if connection:
            connection.rollback()


        return jsonify({

            "error": str(e)

        }), 500


    finally:

        if cursor:
            cursor.close()

        if connection:
            connection.close()


# ============================================================
# GET LATEST ACCIDENT
# GET /api/latest-accident
# ============================================================

@app.route(
    "/api/latest-accident",
    methods=["GET"]
)
def get_latest_accident():

    connection = None
    cursor = None

    try:

        # ----------------------------------------------------
        # Connect to database
        # ----------------------------------------------------

        connection = get_db_connection()

        cursor = connection.cursor(
            dictionary=True
        )


        # ----------------------------------------------------
        # Get latest accident
        # ----------------------------------------------------

        query = """

            SELECT

                a.Accident_ID,

                a.Vehicle_No,

                v.Vehicle_Type,

                v.Owner,

                v.Phone,

                a.Latitude,

                a.Longitude,

                a.Time

            FROM accident_info a

            JOIN vehicle_info v

            ON a.Vehicle_No = v.Vehicle_No

            ORDER BY a.Time DESC

            LIMIT 1

        """


        cursor.execute(query)


        accident = cursor.fetchone()


        # ----------------------------------------------------
        # Return result
        # ----------------------------------------------------

        if accident:

            return jsonify(
                accident
            ), 200


        return jsonify({

            "message":
            "No accident found"

        }), 404


    except Exception as e:

        return jsonify({

            "error": str(e)

        }), 500


    finally:

        if cursor:
            cursor.close()

        if connection:
            connection.close()


# ============================================================
# GET ACCIDENT HISTORY
# GET /api/accidents
# ============================================================

@app.route(
    "/api/accidents",
    methods=["GET"]
)
def get_accidents():

    connection = None
    cursor = None

    try:

        connection = get_db_connection()

        cursor = connection.cursor(
            dictionary=True
        )


        query = """

            SELECT

                a.Accident_ID,

                a.Vehicle_No,

                v.Vehicle_Type,

                v.Owner,

                v.Phone,

                a.Latitude,

                a.Longitude,

                a.Time

            FROM accident_info a

            JOIN vehicle_info v

            ON a.Vehicle_No = v.Vehicle_No

            ORDER BY a.Time DESC

        """


        cursor.execute(query)


        accidents = cursor.fetchall()


        return jsonify(
            accidents
        ), 200


    except Exception as e:

        return jsonify({

            "error": str(e)

        }), 500


    finally:

        if cursor:
            cursor.close()

        if connection:
            connection.close()


# ============================================================
# DASHBOARD STATISTICS
# GET /api/statistics
# ============================================================

@app.route("/api/statistics", methods=["GET"])
def get_statistics():

    connection = None
    cursor = None

    try:

        connection = get_db_connection()

        cursor = connection.cursor(dictionary=True)

        # Total vehicles
        cursor.execute(
            "SELECT COUNT(*) AS total FROM vehicle_info"
        )
        total_vehicles = cursor.fetchone()["total"]

        # Total accidents
        cursor.execute(
            "SELECT COUNT(*) AS total FROM accident_info"
        )
        total_accidents = cursor.fetchone()["total"]

        # Latest accident time
        cursor.execute(
            """
            SELECT Time
            FROM accident_info
            ORDER BY Time DESC
            LIMIT 1
            """
        )

        latest = cursor.fetchone()

        return jsonify({

            "total_vehicles": total_vehicles,

            "total_accidents": total_accidents,

            "live_alerts": 1 if total_accidents > 0 else 0,

            "system_status": "ONLINE",

            "latest_accident":

                latest["Time"].strftime("%d-%m-%Y %H:%M:%S")

                if latest else

                "No accidents"

        })

    except Exception as e:

        return jsonify({
            "error": str(e)
        }),500

    finally:

        if cursor:
            cursor.close()

        if connection:
            connection.close()

# ============================================================
# SYSTEM HEALTH
# ============================================================

@app.route("/api/health")
def health():

    return jsonify({

        "status":"ONLINE",

        "server_time":

            datetime.now().strftime(

                "%d-%m-%Y %H:%M:%S"

            )

    })

@app.route("/api/location/<vehicle_no>")
def location(vehicle_no):

    connection=get_db_connection()

    cursor=connection.cursor(dictionary=True)

    cursor.execute("""

    SELECT Latitude,

           Longitude

    FROM Accident_info

    WHERE Vehicle_No=%s

    ORDER BY Time DESC

    LIMIT 1

    """,(vehicle_no,))

    data=cursor.fetchone()

    cursor.close()

    connection.close()

    return jsonify(data)

# ============================================================
# RUN FLASK APPLICATION
# ============================================================

if __name__ == "__main__":

    socketio.run(

        app,

        debug=True,

        host="0.0.0.0",

        port=5000

    )