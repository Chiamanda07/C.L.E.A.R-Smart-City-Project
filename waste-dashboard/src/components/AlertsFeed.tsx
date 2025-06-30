// src/components/AlertsFeed.tsx
import React from "react";
import type { Alert } from "../types";

export const AlertsFeed: React.FC<{ alerts: Alert[] }> = ({ alerts }) => {
  return (
    <div className="bg-white p-4 rounded shadow col-span-full">
      <h2 className="text-xl font-bold mb-4">Alerts</h2>

      {alerts.length === 0 ? (
        <p className="text-gray-500 italic">No alerts at this time.</p>
      ) : (
        <ul className="divide-y divide-gray-200">
          {alerts.map((alert, i) => (
            <li key={i} className="py-3 flex items-start justify-between">
              <div>
                <p className="text-sm font-semibold text-red-600">
                  🚨 {alert.message}
                </p>
                <p className="text-sm text-gray-700">
                  Bin: <span className="font-mono">{alert.binId}</span>
                </p>
              </div>
              <span className="text-xs text-gray-500 whitespace-nowrap">
                {new Intl.DateTimeFormat("en-US", {
                  dateStyle: "medium",
                  timeStyle: "short",
                }).format(new Date(alert.timestamp))}
              </span>
            </li>
          ))}
        </ul>
      )}
    </div>
  );
};
