import "./App.css";
import { Dashboard } from "./components/Dashboard";
import { mockData } from "./mockData.ts";

function App() {
  return (
    <div className="p-6 font-sans bg-gray-100 min-h-screen w-full">
      <h1 className="text-3xl text-center text-black font-bold mb-4">
        Waste Bin Monitoring Dashboard
      </h1>
      <Dashboard data={mockData} />
    </div>
  );
}

export default App;
