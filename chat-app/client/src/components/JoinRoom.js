import React, { useState } from "react";
import { Link } from "react-router-dom";
export default function JoinRoom() {
	const [roomId, setRoomId] = useState();
	const [nickname, setNickName] = useState("");
	return (
		<div>
			<div className="join-wrapper">
				<input
					onChange={(e) => setRoomId(e.target.value)}
					className="join-key-input"
					placeholder="Enter key"
					type="text"
				></input>
				<Link
					onClick={(e) => (!roomId || !nickname ? e.preventDefault() : null)}
					to={`/chat?roomId=${roomId}&user=${nickname}`}
				>
					<button className="join-btn">join</button>
				</Link>
			</div>
			<input
				onChange={(e) => setNickName(e.target.value)}
				className="quick-nick-input"
				placeholder="Nick name"
			></input>
			<h2 className="join-room-header">Join a room</h2>
		</div>
	);
}
