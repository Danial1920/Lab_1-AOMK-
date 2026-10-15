% Independent MATLAB reference for laboratory task 44.
cases = {
    {[-2 -1 0 1 2], [11 5 3 5 11], 2},
    {[-2 -1 0 1 2 3], [-14 -0.5 3 2.5 4 13.5], 3},
    {[-10 -2 4 9], [7 7 7 7], 0}
};
for k = 1:numel(cases)
    c = cases{k};
    fprintf('degree=%d coefficients=', c{3});
    fprintf(' %.17g', polyfit(c{1}, c{2}, c{3}));
    fprintf('\n');
end
