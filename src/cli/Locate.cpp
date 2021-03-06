#include <utility>

/*
 *  Copyright (C) 2017 KeePassXC Team <team@keepassxc.org>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 or (at your option)
 *  version 3 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <cstdlib>
#include <stdio.h>

#include "Locate.h"

#include <QCommandLineParser>
#include <QStringList>
#include <QTextStream>

#include "cli/Utils.h"
#include "core/Global.h"
#include "core/Database.h"
#include "core/Entry.h"
#include "core/Group.h"

Locate::Locate()
{
    name = QString("locate");
    description = QObject::tr("Find entries quickly.");
}

Locate::~Locate()
{
}

int Locate::execute(const QStringList& arguments)
{
    QTextStream out(Utils::STDOUT, QIODevice::WriteOnly);

    QCommandLineParser parser;
    parser.setApplicationDescription(description);
    parser.addPositionalArgument("database", QObject::tr("Path of the database."));
    parser.addPositionalArgument("term", QObject::tr("Search term."));
    QCommandLineOption keyFile(QStringList() << "k" << "key-file",
                               QObject::tr("Key file of the database."),
                               QObject::tr("path"));
    parser.addOption(keyFile);
    parser.addHelpOption();
    parser.process(arguments);

    const QStringList args = parser.positionalArguments();
    if (args.size() != 2) {
        out << parser.helpText().replace("keepassxc-cli", "keepassxc-cli locate");
        return EXIT_FAILURE;
    }

    QScopedPointer<Database> db(Database::unlockFromStdin(args.at(0), parser.value(keyFile), Utils::STDOUT, Utils::STDERR));
    if (!db) {
        return EXIT_FAILURE;
    }

    return locateEntry(db.data(), args.at(1));
}

int Locate::locateEntry(Database* database, const QString& searchTerm)
{
    QTextStream out(Utils::STDOUT, QIODevice::WriteOnly);
    QTextStream err(Utils::STDERR, QIODevice::WriteOnly);

    QStringList results = database->rootGroup()->locate(searchTerm);
    if (results.isEmpty()) {
        err << "No results for that search term." << endl;
        return EXIT_FAILURE;
    }

    for (const QString& result : asConst(results)) {
        out << result << endl;
    }
    return EXIT_SUCCESS;
}
