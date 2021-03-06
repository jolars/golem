test_that("incorrect data dimensions throw errors", {
  expect_error(owl(matrix(1, 3, 3), double(2)))
})

test_that("na values in input throws errors", {
  expect_error(owl(matrix(NA, 3, 3), double(3)))
  expect_error(owl(matrix(3, 3, 3), c(NA, NA, 1)))
})

test_that("erroneous slope input throws errors", {
  x <- matrix(1, 3, 3)
  y <- double(3)

  expect_error(owl(x, y, lambda = 1:2))
  expect_error(owl(x, y, lambda = -c(1, 2)))
  expect_error(owl(x, y, lambda = 4:1))
})
