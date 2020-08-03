import React, { useState } from "react";
import { Link, Redirect } from "react-router-dom";
import GetRoomId from "../containers/GetRoomId";
export default function CreateRoom() {
	const [roomId, setRoomId] = useState();
	const [nickname, setNickName] = useState("");
	//Generates random room id
	const CreateRoom = () => {
		const res = GetRoomId();
		res
			.then((room) => {
				setRoomId(room.data.roomId);
				console.log("key made!");
			})
			.catch((err) => {
				console.log(err);
			});
	};
	return (
		<div>
			<div className="create-wrapper">
				<Link
					onClick={(e) => (!roomId || !nickname ? e.preventDefault() : null)}
					to={`/chat?roomId=${roomId}&user=${nickname}`}
				>
					<button className="generate-key-btn">Create</button>
				</Link>

				<button onClick={() => CreateRoom()} className="generate-key-btn">
					make key
				</button>
			</div>
			<input
				onChange={(e) => setNickName(e.target.value)}
				className="quick-nick-input"
				placeholder="Nick name"
			></input>
			<h2 className="join-room-header">Create a room</h2>
		</div>
	);
}
