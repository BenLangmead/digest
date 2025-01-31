#pragma once
#include "data_structure.hpp"
#include "digest/digester.hpp"
#include <cstddef>
#include <cstdint>

namespace digest {

class BadWindowSizeException : public std::exception {
  const char *what() const throw() {
    return "Number of kmers in large window cannot be 0";
  }
};

// number of k-mers to be considered in the large window
template <BadCharPolicy P, class T> class WindowMin : public Digester<P> {
public:
  /**
   * @param seq
   * @param len
   * @param k
   * @param large_window
   * @param start
   * @param minimized_h
   *
   * @throws BadWindowException Thrown when congruence is greater or equal to
   * mod
   */
  WindowMin(const char *seq, size_t len, unsigned k, unsigned large_window,
            size_t start = 0,
            MinimizedHashType minimized_h = MinimizedHashType::CANON)
      : Digester<P>(seq, len, k, start, minimized_h), ds(large_window),
        large_window(large_window), ds_size(0), is_minimized(false) {
    if (large_window == 0) {
      throw BadWindowSizeException();
    }
  }

  /**
   * @param seq
   * @param k
   * @param large_window
   * @param start
   * @param minimized_h
   *
   * @throws BadWindowException Thrown when congruence is greater or equal to
   * mod
   */
  WindowMin(const std::string &seq, unsigned k, unsigned large_window,
            size_t start = 0,
            MinimizedHashType minimized_h = MinimizedHashType::CANON)
      : WindowMin<P, T>(seq.c_str(), seq.size(), k, large_window, start,
                        minimized_h) {}

  /**
   * @brief adds up to amount of positions of minimizers into vec, here a k-mer
   * is considered a minimizer if its hash is the smallest in the large window,
   * using rightmost index wins in ties
   *
   * @param amount
   * @param vec
   */
  virtual void roll_minimizer(unsigned amount,
                              std::vector<uint32_t> &vec) override;

  /**
   * @brief adds up to amount of positions and hashes of minimizers into vec,
   * here a k-mer is considered a minimizer if its hash is the smallest in the
   * large window, using rightmost index wins in ties
   *
   * @param amount
   * @param vec
   */
  virtual void
  roll_minimizer(unsigned amount,
                 std::vector<std::pair<uint32_t, uint32_t>> &vec) override;

  unsigned get_large_wind_kmer_am() { return large_window; }

  size_t get_ds_size() { return ds_size; }

  // function is mainly to help with tests
  bool get_is_minimized() { return is_minimized; }

protected:
  // data structure which will find miminum
  T ds;

  uint32_t large_window;

  // internal counter that tracks the number of actual values in the data
  // structure
  size_t ds_size;

  // internal bool keeping track of if we have obtained the first minimizer yet,
  // because we don't want to add a position to the vector if it's already in
  // there
  bool is_minimized;

  // the index of previous minimizer, a minimizer is only a new minimizer if it
  // is different from the previous minimizer
  uint32_t prev_mini;

private:
  /**
   * @brief helper function which handles adding the next hash into the data
   * structure
   *
   */
  void roll_ds_wind(std::vector<uint32_t> &vec);

  /**
   * @brief helper function which handles adding the next hash into the data
   * structure
   *
   */
  void roll_ds_wind(std::vector<std::pair<uint32_t, uint32_t>> &vec);

  /**
   * @brief helper function that checks to see if the current minimizer is a new
   * minimizer, and should thus be added to the vec
   *
   * @param vec
   */
  void check(std::vector<uint32_t> &vec);

  /**
   * @brief helper function that checks to see if the current minimizer is a new
   * minimizer, and should thus be added to the vec
   *
   * @param vec
   */
  void check(std::vector<std::pair<uint32_t, uint32_t>> &vec);
};

} // namespace digest

#include "window_minimizer.tpp"
