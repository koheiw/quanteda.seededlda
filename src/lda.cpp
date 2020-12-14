#include "lib.h"
#include "dev.h"
#include "lda.h"
using namespace quanteda;
using namespace arma;
using namespace std;
using namespace Rcpp;

// [[Rcpp::export]]
List cpp_lda(arma::sp_mat &mt, int k, int max_iter, double alpha, double beta,
                  arma::sp_mat &seeds, bool verbose) {
    LDA lda;
    lda.set_data(mt);
    lda.seed = floor(Rcpp::runif(1)[0] * std::numeric_limits<int>::max());
    lda.K = k;
    if (max_iter > 0)
        lda.niters = max_iter;
    if (alpha > 0)
        lda.alpha = alpha;
    if (beta > 0)
        lda.beta = beta;
    if (verbose)
        lda.verbose = verbose;
    if (lda.init_est() == 0) {
        bool seeded = arma::accu(seeds) > 0;
        arma::umat s;
        if (seeded) {
            if (arma::size(seeds) != arma::size(lda.nw))
                std::invalid_argument("Invalid seed matrix");
            s = arma::conv_to<arma::umat>::from(arma::mat(seeds));
            lda.nw = lda.nw + s; // set pseudo count
            //lda.nwsum = lda.nwsum + arma::sum(s, 0);
        }
        lda.estimate();
        if (seeded)
            lda.nwsum = lda.nwsum + arma::sum(s, 0);
    }
    lda.compute_theta();
    lda.compute_phi();

    return List::create(Rcpp::Named("k") = lda.K,
                        Rcpp::Named("iter") = lda.liter,
                        Rcpp::Named("alpha") = lda.alpha,
                        Rcpp::Named("beta") = lda.beta,
                        Rcpp::Named("phi") = wrap(lda.phi),
                        Rcpp::Named("theta") = wrap(lda.theta));
}
