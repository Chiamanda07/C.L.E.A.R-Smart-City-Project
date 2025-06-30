// src/types.ts
export type BinStatus = "normal" | "warning" | "critical";

export interface Bin {
  id: string;
  location: {
    name: string;
    lat: number;
    lng: number;
  };
  fullness: number;
  lastUpdated: string;
  status: BinStatus;
}

export interface Alert {
  binId: string;
  message: string;
  timestamp: string;
}

export interface Summary {
  totalBins: number;
  binsFull: number;
  binsWarning: number;
  binsNormal: number;
  avgFullness: number;
}

export interface DashboardData {
  bins: Bin[];
  alerts: Alert[];
  summary: Summary;
}
