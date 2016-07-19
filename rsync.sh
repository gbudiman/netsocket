sed -i '' 's/localhost/nunki.usc.edu/' main.h
rsync -rv --exclude ".git/" --exclude "*.gch" --exclude "*.o" * gbudiman@nunki.usc.edu:netsocket/
sed -i '' 's/nunki.usc.edu/localhost/' main.h