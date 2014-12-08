CLIENT_DSC = tegclient.dsc
SERVER_DSC = teg.dsc
SERVER_ROOM = teg.room

ggzdgamesdir=$(ggzdconfdir)/games
ggzdroomsdir=$(ggzdconfdir)/rooms

install-data-local:
	$(GGZ_CONFIG) -D --force --install --modfile=$(CLIENT_DSC)
	$(INSTALL_DATA) $(SERVER_DSC) $(ggzdgamesdir)
	$(INSTALL_DATA) $(SERVER_ROOM) $(ggzdroomsdir)

uninstall:
	$(GGZ_CONFIG) --remove --modfile=$(CLIENT_DSC)
	rm -f $(ggzdgamesdir)/$(SERVER_DSC)
	rm -f $(ggzdroomsdir)/$(SERVER_ROOM)

EXTRA_DIST= $(SERVER_DSC).in $(SERVER_ROOM) $(CLIENT_DSC).in README
