#include "vatic_csv.hpp"

using namespace std;

std::vector<PairedTrade> run_main_loop(QuoteTime & quotes,
				       std::vector<TradesRow> & trades) {

  unordered_map<std::string, TradesQueue> trades_qm;

  std::vector<PairedTrade> pairs;
  pairs.reserve(10*1000);

  for(auto & trade: trades) {
    quotes.update_info(trade.time);
    std::string & symbol = trade.symbol;
    trade.q_bid = quotes.bid(symbol);
    trade.q_ask = quotes.ask(symbol);
    
    auto it = trades_qm.find(symbol);

    if (it == trades_qm.end()){
      trades_qm[symbol] = TradesQueue(trade);
    }
    else {
      (it -> second).push_trade(trade, pairs, symbol, quotes); // pairs is possibly modifed
    }    
  }
  return pairs;
}


int main(int argc, char** argv) {
  std::string quotes_csv_file, trades_csv_file;

  if (argc == 3) {
    quotes_csv_file = argv[1];
    trades_csv_file = argv[2];
  }

  // Parse both CSV files
  std::vector<TradesRow> trades {parse_trades(trades_csv_file)};
  std::vector<QuotesRow> quotes {parse_quotes(quotes_csv_file)};

  // Class to Encapsulate Quotes at a time
  QuoteTime quote_map(std::move(quotes));

  const std::vector<PairedTrade> & paired_trades{run_main_loop(quote_map, trades)};
  output_all_pairs((paired_trades));
}
