function ax=niceaxis(x,y)
maxx = max(max(x));
maxy = max(max(y));
minx = min(min(x));
miny = min(min(y));

dely = maxy - miny;
maxy = maxy + dely/10;
miny = miny - dely/10;

ax = [minx maxx miny maxy];
