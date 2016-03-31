#include <iostream>
#include <sstream>
#include <unordered_map>
#include <queue>
#include <cmath>
#include <string>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>

using namespace std;

struct PairedTrade {
  int open_time;
  int close_time;
  std::string symbol;
  int quantity;
  double pnl;
  char open_side;
  char close_side;
  double open_price;
  double close_price;
  double open_bid;
  double close_bid;
  double open_ask;
  double close_ask;
  char open_liq;
  char close_liq;

  inline void to_csv() const {

    std::cout << open_time << ","
	      << close_time << ","
	      << symbol << ","
	      << quantity << ","
	      << pnl << ","
	      << open_side << ","
	      << close_side << ","
	      << open_price << ","
	      << close_price << ","
	      << open_bid << ","
	      << close_bid << ","
	      << open_ask << ","
	      << close_ask << ","
	      << open_liq << ","
	      << close_liq;
  }
};

struct TradesRow {
  int time; // time has to be less than 5*10^6 by constraints
  std::string symbol;  // instructions were not clear about symbol size (3 chars?)
  char side; // buy or sell
  double price; // 2-decimal place precision
  int quantity; // will assume this can fit in an int

  double q_bid;
  double q_ask;
  
  TradesRow() {}
  TradesRow(int t, std::string s, char id, double p, int q) :
    time(t), symbol(s), side(id), price(p), quantity(q), q_bid(0.0), q_ask(0.0) {}
};

struct QuotesRow {
  int time;
  std::string symbol;
  double bid;
  double ask;
};

struct QuotesData {
  int time;
  double bid;
  double ask;
};

class QuoteTime {
  unordered_map<std::string, QuotesRow*> info;
  std::vector<QuotesRow> quotes;
  int cur_time;
  int cur_idx;
public:
  QuoteTime(std::vector<QuotesRow> quotes) : quotes(quotes), cur_time(0),
					     cur_idx(0) {}

  void update_info(int time) {
    while (cur_idx < quotes.size() && quotes[cur_idx].time < time) {
      info[quotes[cur_idx].symbol] = &quotes[cur_idx];
      cur_time = quotes[cur_idx].time;
      ++cur_idx;
    }
    if (cur_idx >= quotes.size())
      --cur_idx;
  }

  size_t sz() const { return quotes.size(); }

  double bid(std::string & symbol) { return (info[symbol])->bid;}
  double ask(std::string & symbol) { return (info[symbol])->ask;}
  
};


enum class Liquidity {A, P}; // aggressive and passive
const std::string delim = ",";

class TradesQueue {
private:
  char side; // buy or sell
  std::queue<TradesRow*> trade_queue;
public:
  TradesQueue(TradesRow& row): side(row.side), trade_queue() {
    trade_queue.push(&row);
  }
  TradesQueue() {}
  void push_trade(TradesRow & row,
		  std::vector<PairedTrade> & pairs,
		  std::string & symbol,
		  QuoteTime & quotes) {
    // compare type of row to type of this TradesQueue
    // if equal, then we just push the trade into the queue
    // else we collapse the trade unto the queue until destroyed
    // and we add the record to the paired trades
    if (side == 'N' || row.side == side) {
      trade_queue.push(&row);
    }
    else {
      bool consumed = false;
      while(!trade_queue.empty() && !consumed) {
	auto & early_t = *(trade_queue.front());
	int quant_diff = row.quantity - early_t.quantity;
	int quant_amt = 0;
	if (quant_diff >= 0) {
	  // delete the earliest trade since this one consumes it
	  row.quantity = quant_diff;
	  quant_amt = early_t.quantity;
	  trade_queue.pop();
	}
	else { 
	  quant_amt = row.quantity;
	  row.quantity = 0;	  
	  consumed = true;	  
	}
	
	PairedTrade pair;
	pair.open_time = early_t.time;
	pair.close_time = row.time;
	pair.symbol = symbol;
	pair.quantity = quant_amt;
	pair.pnl = pair.quantity*(row.price - early_t.price);
	pair.open_liq = 'P';
	pair.close_liq = 'P';

	if(side == 'S') {
	  pair.pnl *= -1.0;

	  if (early_t.price <= early_t.q_bid)
	    pair.open_liq = 'A';

	  if (row.price >= row.q_ask)
	    pair.close_liq = 'A';
	    
	}
	else {
	  if (early_t.price >= early_t.q_ask)
	    pair.open_liq = 'A';

	  if (row.price <= row.q_bid)
	    pair.close_liq = 'A';
	}

	pair.open_side = side;
	pair.close_side = row.side;

	pair.open_price = early_t.price;
	pair.close_price = row.price;

	pair.open_bid = early_t.q_bid;
	pair.close_bid = row.q_bid;

	pair.open_ask = early_t.q_ask;
	pair.close_ask = row.q_ask;	
	  
	pairs.emplace_back(pair);
      }
      if (row.quantity > 0) {
	side = row.side; // switch the queue's side
	trade_queue.push(&row);
      }
      else if (trade_queue.empty()) 
	side = 'N';
    }
  }
};

void output_all_pairs(const std::vector<PairedTrade> & trades) {
  std::cout << std::fixed
	    << std::setprecision(2);  

  for(const auto & trade: trades) {
    trade.to_csv();
    std::cout << "\n";
  }
}
