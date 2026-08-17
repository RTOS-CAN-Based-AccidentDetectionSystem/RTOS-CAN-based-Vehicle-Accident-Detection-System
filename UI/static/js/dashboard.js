// ============================================================
// CONNECT TO FLASK-SOCKETIO
// ============================================================

const socket = io();


// ============================================================
// GLOBAL MAP VARIABLES
// ============================================================

let map;

let accidentMarker = null;


// ============================================================
// INITIALIZE DASHBOARD
// ============================================================

document.addEventListener(
    "DOMContentLoaded",
    function () {

        initializeMap();

        loadVehicleCount();

        loadAccidentHistory();

        loadLatestAccident();

    }
);


// ============================================================
// INITIALIZE MAP
// ============================================================

function initializeMap() {

    // Default location: Pune
    // This is only the initial map center.

    map = L.map(
        "map"
    ).setView(
        [
            18.5204,
            73.8567
        ],
        12
    );


    // OpenStreetMap tiles

    L.tileLayer(

        "https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png",

        {

            maxZoom: 19,

            attribution:
                "&copy; OpenStreetMap contributors"

        }

    ).addTo(map);

}


// ============================================================
// LOAD VEHICLE COUNT
// ============================================================

async function loadVehicleCount() {

    try {

        const response = await fetch(
            "/api/vehicles"
        );


        const vehicles =
            await response.json();


        document.getElementById(
            "vehicle-count"
        ).textContent =
            vehicles.length;


    } catch (error) {

        console.error(
            "Error loading vehicles:",
            error
        );

    }

}


// ============================================================
// LOAD ACCIDENT HISTORY
// ============================================================

async function loadAccidentHistory() {

    try {

        const response = await fetch(
            "/api/accidents"
        );


        const accidents =
            await response.json();


        // Update total accidents

        document.getElementById(
            "accident-count"
        ).textContent =
            accidents.length;


        // Calculate today's accidents

        const today =
            new Date().toDateString();


        const todayAccidents =
            accidents.filter(
                accident => {

                    if (!accident.Time) {

                        return false;

                    }


                    const accidentDate =
                        new Date(
                            accident.Time
                        );


                    return (
                        accidentDate.toDateString()
                        ===
                        today
                    );

                }
            );


        document.getElementById(
            "today-count"
        ).textContent =
            todayAccidents.length;


        // Get table

        const tableBody =
            document.getElementById(
                "accident-table-body"
            );


        tableBody.innerHTML = "";


        // No accidents

        if (
            accidents.length === 0
        ) {

            tableBody.innerHTML = `

                <tr>

                    <td
                        colspan="8"
                        class="empty-table"
                    >

                        No accident records found

                    </td>

                </tr>

            `;

            return;

        }


        // Add accident rows

        accidents.forEach(
            accident => {

                const row =
                    document.createElement(
                        "tr"
                    );


                row.innerHTML = `

                    <td>
                        ${accident.Accident_ID}
                    </td>

                    <td>
                        ${accident.Vehicle_No}
                    </td>

                    <td>
                        ${accident.Vehicle_Type || "-"}
                    </td>

                    <td>
                        ${accident.Owner || "-"}
                    </td>

                    <td>
                        ${accident.Phone || "-"}
                    </td>

                    <td>
                        ${Number(
                            accident.Latitude
                        ).toFixed(5)}
                    </td>

                    <td>
                        ${Number(
                            accident.Longitude
                        ).toFixed(5)}
                    </td>

                    <td>
                        ${formatTime(
                            accident.Time
                        )}
                    </td>

                `;


                tableBody.appendChild(
                    row
                );

            }
        );


    } catch (error) {

        console.error(
            "Error loading accidents:",
            error
        );

    }

}


// ============================================================
// LOAD LATEST ACCIDENT
// ============================================================

async function loadLatestAccident() {

    try {

        const response = await fetch(
            "/api/latest-accident"
        );


        if (!response.ok) {

            return;

        }


        const accident =
            await response.json();


        displayLatestAccident(
            accident
        );


    } catch (error) {

        console.error(
            "Error loading latest accident:",
            error
        );

    }

}


// ============================================================
// DISPLAY LATEST ACCIDENT
// ============================================================

function displayLatestAccident(
    accident
) {

    document.getElementById(
        "latest-id"
    ).textContent =
        accident.Accident_ID;


    document.getElementById(
        "latest-vehicle"
    ).textContent =
        accident.Vehicle_No;


    document.getElementById(
        "latest-owner"
    ).textContent =
        accident.Owner;


    document.getElementById(
        "latest-phone"
    ).textContent =
        accident.Phone;


    document.getElementById(
        "latest-latitude"
    ).textContent =
        accident.Latitude;


    document.getElementById(
        "latest-longitude"
    ).textContent =
        accident.Longitude;


    document.getElementById(
        "latest-time"
    ).textContent =
        formatTime(
            accident.Time
        );


    updateMap(

        Number(
            accident.Latitude
        ),

        Number(
            accident.Longitude
        ),

        accident.Vehicle_No

    );

}


// ============================================================
// SOCKETIO - REAL-TIME ACCIDENT
// ============================================================

socket.on("new_accident", function(response){

    console.log("Socket Data:", response);

    const data = response.data;

    showAccidentAlert(data);

    document.getElementById("latest-id").textContent = data.accident_id;
    document.getElementById("latest-vehicle").textContent = data.vehicle_no;
    document.getElementById("latest-owner").textContent = data.owner;
    document.getElementById("latest-phone").textContent = data.phone;
    document.getElementById("latest-latitude").textContent = data.latitude;
    document.getElementById("latest-longitude").textContent = data.longitude;
    document.getElementById("latest-time").textContent = new Date().toLocaleString();

    updateMap(
        Number(data.latitude),
        Number(data.longitude),
        data.vehicle_no
    );

    loadAccidentHistory();

});


// ============================================================
// SHOW ACCIDENT ALERT
// ============================================================

function showAccidentAlert(
    data
) {

    // Hide "No Active Accident"

    document.getElementById(
        "no-alert"
    ).classList.add(
        "hidden"
    );


    // Show accident alert

    document.getElementById(
        "accident-alert"
    ).classList.remove(
        "hidden"
    );


    // Fill data

    document.getElementById(
        "alert-vehicle"
    ).textContent =
        data.vehicle_no;


    document.getElementById(
        "alert-type"
    ).textContent =
        data.vehicle_type || "-";


    document.getElementById(
        "alert-owner"
    ).textContent =
        data.owner || "-";


    document.getElementById(
        "alert-phone"
    ).textContent =
        data.phone || "-";


    document.getElementById(
        "alert-latitude"
    ).textContent =
        data.latitude;


    document.getElementById(
        "alert-longitude"
    ).textContent =
        data.longitude;


    document.getElementById(
        "alert-time"
    ).textContent =
        new Date().toLocaleString();


    // Scroll to alert

    document.getElementById(
        "live-alert"
    ).scrollIntoView({

        behavior: "smooth"

    });

}


// ============================================================
// UPDATE MAP
// ============================================================

function updateMap(

    latitude,

    longitude,

    vehicleNo

) {

    // Validate coordinates

    if (

        isNaN(latitude) ||

        isNaN(longitude)

    ) {

        return;

    }


    // Move map

    map.setView(

        [
            latitude,
            longitude
        ],

        15

    );


    // Remove old marker

    if (
        accidentMarker
    ) {

        map.removeLayer(
            accidentMarker
        );

    }


    // Add new marker

    accidentMarker =
        L.marker(

            [
                latitude,
                longitude
            ]

        ).addTo(
            map
        );


    // Marker popup

    accidentMarker.bindPopup(

        `

        <strong>
            🚨 Accident Location
        </strong>

        <br>

        Vehicle:
        ${vehicleNo}

        <br>

        Latitude:
        ${latitude}

        <br>

        Longitude:
        ${longitude}

        `

    ).openPopup();

}


// ============================================================
// VIEW LOCATION BUTTON
// ============================================================

document.getElementById(
    "view-location-btn"
).addEventListener(

    "click",

    function () {

        document.getElementById(
            "map-section"
        ).scrollIntoView({

            behavior: "smooth"

        });

    }

);


// ============================================================
// SIMULATE ACCIDENT
// ============================================================

document.getElementById(
    "simulate-btn"
).addEventListener(

    "click",

    async function () {


        // IMPORTANT:
        // Change this vehicle number to
        // a vehicle that already exists
        // in your vehicle_info table.

        const accidentData = {

            vehicle_no:
                "MH12AB1234",

            latitude:
                18.5204,

            longitude:
                73.8567

        };


        try {


            const response =
                await fetch(

                    "/api/accident",

                    {

                        method:
                            "POST",

                        headers: {

                            "Content-Type":
                                "application/json"

                        },

                        body:
                            JSON.stringify(
                                accidentData
                            )

                    }

                );


            const result =
                await response.json();


            if (
                response.ok
            ) {

                console.log(
                    "Accident simulated:",
                    result
                );

            } else {

                alert(
                    result.error ||
                    "Failed to simulate accident"
                );

            }


        } catch (error) {

            console.error(
                "Simulation error:",
                error
            );


            alert(
                "Could not connect to Flask server"
            );

        }

    }

);


// ============================================================
// FORMAT TIME
// ============================================================

function formatTime(
    time
) {

    if (!time) {

        return "-";

    }


    const date =
        new Date(
            time
        );


    if (
        isNaN(
            date.getTime()
        )
    ) {

        return time;

    }


    return date.toLocaleString();

}