#include "vatic_infra.hpp"

std::vector<TradesRow> parse_trades(const std::string & trades_csv_file) {
  /**
     Parse Trades CSV file and return a vector of trades
     Expects header line {TIME, SYMBOL, SIDE, PRICE, QUANTITY}
   **/
  
  std::string line;
  ifstream trades_stream(trades_csv_file.c_str());

  getline(trades_stream,line); // skip first line
  std::vector<TradesRow> trades;
  trades.reserve(5*1000*1000);
  
  while(getline(trades_stream,line)) {
      std::stringstream linestream(line);
      std::string value;
      TradesRow row;
      
      
      getline(linestream,value,',');
      row.time = std::stoi(std::move(value));

      getline(linestream,value,',');
      row.symbol = std::move(value);


      getline(linestream,value,',');
      row.side = (value == "B") ? 'B' : 'S';

      getline(linestream,value,',');
      row.price = std::stod(value);

      getline(linestream,value,',');
      row.quantity = std::stoi(value);

      trades.emplace_back(row);
    }
  return std::move(trades);
}

std::vector<QuotesRow> parse_quotes(const std::string & quotes_csv_file) {  
  ifstream quotes_stream(quotes_csv_file.c_str());
  std::string line;
  getline(quotes_stream,line);
  std::vector<QuotesRow> quotes;
  quotes.reserve(5*1000*1000); // max possible quotes size
  while(getline(quotes_stream,line)){
      std::stringstream linestream(line);
      QuotesRow row;
      std::string value;              

      getline(linestream,value,',');
      row.time = std::stoi(std::move(value));

      getline(linestream,value,',');
      row.symbol = std::move(value);

      getline(linestream,value,',');
      row.bid = std::stod(value);

      getline(linestream,value,',');
      row.ask = std::stod(value);

      quotes.emplace_back(row);
    }
  return std::move(quotes);
}
