import axios from "axios";
const GetRoomId = async () => {
	const res = await axios({
		url: "http://localhost:5000",
		method: "GET",
		headers: {
			"Content-Type": "application/json",
		},
	});
	return res;
};
export default GetRoomId;
