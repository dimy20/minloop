import React from "react";
import Styles from "./User.module.css";
export default function Users(props) {
	const { users } = props;
	return (
		<div className={Styles.container}>
			<div className={Styles.wrapper}>
				<h2 style={{ color: "#7289da", marginBottom: "0.5em" }}>users</h2>
				{users &&
					users.map((u) => {
						return (
							<div key={u} className={Styles.userItem}>
								<div className={Styles.circle}></div>
								<p className={Styles.userText}>{u}</p>
							</div>
						);
					})}
			</div>
		</div>
	);
}
