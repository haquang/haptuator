function out = shift( in, Sample )

    for i = 1: (size(in)-Sample)
        out(i) = in(i+Sample);
    end

end

