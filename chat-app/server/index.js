/* require("dotenv").config(".");
const express = require("express");
const app = express();
const morgan = require("morgan");
const cors = require("cors");
const bodyParser = require("body-parser");
const { v4: uuidV4 } = require("uuid");
const corsOptions = {
	credentials: true, // This is important.
	origin: "http://localhost:3000",
};

const room = require("./routes/room");

app.use(cors(corsOptions));
app.use(
	bodyParser.urlencoded({
		extended: false,
	})
);
app.use(morgan("tiny"));
app.all("/", (req, res, next) => {
	res.redirect(`/room/${uuidV4()}`);
});
app.use("/room/", room);

module.exports = app; */
