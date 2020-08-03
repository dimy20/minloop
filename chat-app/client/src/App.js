import React from "react";
import "./App.css";
import Chat from "./containers/Chat";
import { Route, BrowserRouter } from "react-router-dom";
//components
import Join from "./components/Join";

function App() {
	//const roomId = useGetRoomId();
	return (
		<div className="App">
			<BrowserRouter>
				<Route path="/" exact component={Join}></Route>
				<Route path="/chat" exact component={Chat}></Route>
			</BrowserRouter>
		</div>
	);
}

export default App;
