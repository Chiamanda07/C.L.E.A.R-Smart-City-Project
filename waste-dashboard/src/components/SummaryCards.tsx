// src/components/SummaryCards.tsx
import React from "react";
import type { Summary } from "../types";
import {
  CheckCircle,
  AlertTriangle,
  XCircle,
  ListChecks,
  Gauge,
} from "lucide-react";

export const SummaryCards: React.FC<{ summary: Summary }> = ({ summary }) => {
  const cards = [
    {
      title: "Total Bins",
      value: summary.totalBins,
      color: "bg-gray-100 text-gray-700",
      icon: <ListChecks className="w-6 h-6 text-gray-500" />,
    },
    {
      title: "Full Bins",
      value: summary.binsFull,
      color: "bg-red-100 text-red-700",
      icon: <XCircle className="w-6 h-6 text-red-500" />,
    },
    {
      title: "Warning Bins",
      value: summary.binsWarning,
      color: "bg-yellow-100 text-yellow-700",
      icon: <AlertTriangle className="w-6 h-6 text-yellow-500" />,
    },
    {
      title: "Normal Bins",
      value: summary.binsNormal,
      color: "bg-green-100 text-green-700",
      icon: <CheckCircle className="w-6 h-6 text-green-500" />,
    },
    {
      title: "Average Fullness",
      value: `${summary.avgFullness.toFixed(1)}%`,
      color: "bg-blue-100 text-blue-700",
      icon: <Gauge className="w-6 h-6 text-blue-500" />,
    },
  ];

  return (
    <div className="col-span-full grid grid-cols-2 sm:grid-cols-3 lg:grid-cols-5 gap-4">
      {cards.map((card) => (
        <div
          key={card.title}
          className={`p-4 rounded shadow flex items-center justify-between ${card.color}`}
        >
          <div>
            <h3 className="text-sm font-medium">{card.title}</h3>
            <p className="text-xl font-bold">{card.value}</p>
          </div>
          <div className="ml-4">{card.icon}</div>
        </div>
      ))}
    </div>
  );
};
