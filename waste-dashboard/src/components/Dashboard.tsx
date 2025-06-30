import type { DashboardData } from "../types";
import { AlertsFeed } from "./AlertsFeed";
import { BinMap } from "./BinMap";
import { FullnessBarChart } from "./FullnessBarChart";
import { StatusPieChart } from "./StatusPieChart";
import { SummaryCards } from "./SummaryCards";

interface Props {
  data: DashboardData;
}

export const Dashboard: React.FC<Props> = ({ data }) => {
  return (
    <div className="flex flex-col justify-between gap-2">
      <SummaryCards summary={data.summary} />
      <div className="py-4 w-full flex flex-col items-center md:flex-row gap-4">
        <div className="w-full md:w-2/4">
          <StatusPieChart summary={data.summary} />
        </div>
        <div className="w-full md:w-2/4">
          <FullnessBarChart bins={data.bins} />
        </div>
      </div>
      <BinMap bins={data.bins} alerts={data.alerts} />
      <AlertsFeed alerts={data.alerts} />
    </div>
  );
};
