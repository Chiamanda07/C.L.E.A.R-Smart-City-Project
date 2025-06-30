// src/components/FullnessBarChart.tsx
import React from "react";
import { Bar } from "react-chartjs-2";
import {
  Chart as ChartJS,
  CategoryScale,
  LinearScale,
  BarElement,
  Tooltip,
  Legend,
} from "chart.js";
import type { Bin } from "../types";

ChartJS.register(CategoryScale, LinearScale, BarElement, Tooltip, Legend);

export const FullnessBarChart: React.FC<{ bins: Bin[] }> = ({ bins }) => {
  const data = {
    labels: bins.map((bin) => bin.location.name),
    datasets: [
      {
        label: "Fullness Percentage",
        data: bins.map((bin) => bin.fullness),
        backgroundColor: bins.map((bin) =>
          bin.status === "normal"
            ? "#22c55e"
            : bin.status === "warning"
            ? "#facc15"
            : "#ef4444"
        ),
      },
    ],
  };
  return (
    <Bar
      data={data}
      options={{ scales: { y: { beginAtZero: true, max: 100 } } }}
    />
  );
};
