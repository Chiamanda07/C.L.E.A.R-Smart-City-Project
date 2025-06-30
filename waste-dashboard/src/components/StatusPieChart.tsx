import React from "react";
import { Pie } from "react-chartjs-2";
import { Chart as ChartJS, ArcElement, Tooltip, Legend } from "chart.js";
import type { Summary } from "../types";

ChartJS.register(ArcElement, Tooltip, Legend);

const options = {
  plugins: {
    responsive: true,
    maintainAspectRatio: false,
    legend: {
      position: "right" as const, // <-- move legend to the right
      labels: {
        boxWidth: 20,
        padding: 20,
      },
    },
  },
};

export const StatusPieChart: React.FC<{ summary: Summary }> = ({ summary }) => {
  const data = {
    labels: ["Normal", "Warning", "Full"],
    datasets: [
      {
        label: "Bin Status",
        data: [summary.binsNormal, summary.binsWarning, summary.binsFull],
        backgroundColor: ["#22c55e", "#facc15", "#ef4444"],
      },
    ],
  };
  return <Pie data={data} options={options} />;
};
