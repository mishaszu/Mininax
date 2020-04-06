let applyTuple3 = (f, ~tuple) => {
  let (x, y, z) = tuple;
  f(x, y, z);
};

let applyQuery = (f, ~query) => f(query);
let apply1 = (f, param) => f(param);
let applyField = (f, ~field) => f(field);
let toOption = value => Some(value);
let toResult = value => Ok(value);
let slice = (value: string, ~to_: int) =>
  value |> Js.String.slice(~from=0, ~to_) |> Js.String.toLowerCase;

let applyTuple3SkipSecond = (f, ~tuple) => {
  let (x, _, z) = tuple;
  f(x, z);
};

let labelExecuteEntityQuery = (info, platform, network, ~field, query) =>
  ConseiljsRe.ConseilDataClient.executeEntityQuery(
    info,
    platform,
    network,
    field,
    query,
  );

let labelGetBlock = (conseilServerInfo, network, ~id) =>
  ConseiljsRe.TezosConseilClient.getBlock(conseilServerInfo, network, id);

let getBlockTotalsThunk = (~id: string, ~config: MainType.config) =>
  labelExecuteEntityQuery(~field="operations")
  ->applyTuple3(~tuple=Utils.getInfo(config))
  ->applyQuery(~query=Queries.getQueryForBlockTotals(id))
  ->FutureJs.fromPromise(_err => None)
  ->Future.flatMap(
      fun
      | Ok(totals) when totals |> Js.Array.length > 0 =>
        Future.value(Some(totals[0]))
      | _err => Future.value(None),
    );

let getBlockHeadThunk = (~callback, ~config: MainType.config) =>
  ConseiljsRe.TezosConseilClient.getBlockHead
  ->applyTuple3SkipSecond(~tuple=Utils.getInfo(config))
  ->FutureJs.fromPromise(_err => None)
  ->Future.map(
      fun
      | Ok(result) => Some(result)
      | _err => None,
    )
  ->Future.get(callback);

let getBlockFromApi = (~id: string, ~config: MainType.config) =>
  labelGetBlock(~id)
  ->applyTuple3SkipSecond(~tuple=Utils.getInfo(config))
  ->FutureJs.fromPromise(_err => None)
  ->Future.flatMap(
      fun
      | Ok(block) => Future.value(Some(block))
      | _err => Future.value(None),
    );

let getBlockThunk = (~callback, ~id: string, ~config: MainType.config) =>
  Js.Promise.all2((
    FutureJs.toPromise(getBlockFromApi(~id, ~config)),
    FutureJs.toPromise(getBlockTotalsThunk(~id, ~config)),
  ))
  ->FutureJs.fromPromise(_err => None)
  ->Future.map(
      fun
      | Ok((Some(block), Some(total))) => {
          let realBlock = Utils.convertBlock(~block, ~total, ());
          Ok((realBlock, block));
        }
      | Ok((Some(block), None)) => {
          let realBlock = Utils.convertBlock(~block, ());
          Ok((realBlock, block));
        }
      | Ok((None, None)) => Error(Utils.noAvailable)
      | _err => Error(Utils.invalidId),
    )
  ->Future.get(callback);

// TODO Started here
let getBlockHashThunk = (~callback, ~level: int, ~config: MainType.config) =>
  ConseiljsRe.TezosConseilClient.getBlockByLevel
  ->applyTuple3SkipSecond(~tuple=Utils.getInfo(config))
  ->apply1(level)
  ->FutureJs.fromPromise(_err => None)
  ->Future.map(
      fun
      | Ok(hashes) when hashes |> Js.Array.length > 0 => Some(hashes[0])
      | _err => None,
    )
  ->Future.get(callback);

let getAccountBakerThunk = (~id: string, ~config: MainType.config) =>
  ConseiljsRe.ConseilDataClient.executeEntityQuery
  ->applyTuple3(~tuple=Utils.getInfo(config))
  ->applyField(~field="delegates")
  ->applyQuery(~query=Queries.getQueryForBakerInfo(id))
  ->FutureJs.fromPromise(_err => None)
  ->Future.flatMap(
      fun
      | Ok(delegates) when delegates |> Js.Array.length > 0 =>
        Future.value(Some(delegates[0]))
      | _err => Future.value(None),
    );

let getOperationThunk = (~callback, ~id: string, ~config: MainType.config) =>
  ConseiljsRe.TezosConseilClient.getOperations
  ->applyTuple3SkipSecond(~tuple=Utils.getInfo(config))
  ->applyQuery(~query=Queries.getQueryForOperations(id))
  ->FutureJs.fromPromise(_err => Utils.noAvailable)
  ->Future.map(
      fun
      | Ok(operations) when operations |> Js.Array.length > 0 =>
        operations |> Array.map(Utils.convertOperation) |> toResult
      | Ok(_) => Error(Utils.noAvailable)
      | Error(e) => Error(e),
    )
  ->Future.get(callback);

let getAccountFromApi = (~id: string, ~config: MainType.config) =>
  ConseiljsRe.TezosConseilClient.getAccount
  ->applyTuple3SkipSecond(~tuple=Utils.getInfo(config))
  ->apply1(id)
  ->FutureJs.fromPromise(_err => None)
  ->Future.flatMap(
      fun
      | Ok(account) => Some(account) |> Future.value
      | _err => None |> Future.value,
    );

let getAccountThunk = (~callback, ~id: string, ~config: MainType.config) =>
  Js.Promise.all2((
    getAccountFromApi(~id, ~config) |> FutureJs.toPromise,
    getAccountBakerThunk(~id, ~config) |> FutureJs.toPromise,
  ))
  ->FutureJs.fromPromise(_err => Utils.noAvailable)
  ->Future.map(
      fun
      | Ok((Some(account), Some(baker))) =>
        Utils.convertAccount(~account, ~baker, ()) |> toResult
      | Ok((Some(account), None)) =>
        Utils.convertAccount(~account, ()) |> toResult
      | Ok(_) when id |> slice(~to_=2) == "tz" => Error(Utils.missingAccount)
      | Ok(_) when id |> slice(~to_=2) == "kt" =>
        Error(Utils.missingContract)
      | Ok(_) => Error(Utils.invalidAccountId)
      | Error(err) => Error(err),
    )
  ->Future.get(callback);

let getForQueryApi = (~query, ~field: string, ~config: MainType.config) =>
  ConseiljsRe.ConseilDataClient.executeEntityQuery
  ->applyTuple3(~tuple=Utils.getInfo(config))
  ->applyField(~field)
  ->applyQuery(~query)
  ->FutureJs.fromPromise(_err => None)
  ->Future.flatMap(
      fun
      | Ok(value) when value |> Js.Array.length > 0 =>
        value[0] |> toOption |> Future.value
      | _ => None |> Future.value,
    );

// getQuorumInfoApi = blocks = getQueryForQuorum
// getVotingStatsApi = governance = getQueryForVotingStats
let getVoteInfoThunk =
    (
      ~callback,
      ~hash: string,
      ~active_proposal: string,
      ~config: MainType.config,
    ) =>
  Js.Promise.all2((
    getForQueryApi(
      ~query=Queries.getQueryForQuorum(hash),
      ~field="blocks",
      ~config,
    )
    |> FutureJs.toPromise,
    getForQueryApi(
      ~query=Queries.getQueryForVotingStats(hash, active_proposal),
      ~field="governance",
      ~config,
    )
    |> FutureJs.toPromise,
  ))
  ->FutureJs.fromPromise(_err => None)
  ->Future.map(
      fun
      | Ok((Some(quorumStat), Some(votingStat))) => {
          let quorumObj = quorumStat |> Obj.magic;
          let votingObj = votingStat |> Obj.magic;
          let votinfo: MainType.voteInfo = {
            yay_rolls: votingObj##yay_rolls,
            nay_rolls: votingObj##nay_rolls,
            pass_rolls: votingObj##pass_rolls,
            proposal_hash: votingObj##proposal_hash,
            current_expected_quorum: quorumObj##current_expected_quorum,
          };
          Some(votinfo);
        }
      | _ => None,
    )
  ->Future.get(callback);

let getProposalInfoThunk =
    (~callback, ~metaCycle: int, ~config: MainType.config) =>
  ConseiljsRe.ConseilDataClient.executeEntityQuery
  ->applyTuple3(~tuple=Utils.getInfo(config))
  ->applyField(~field="operations")
  ->applyQuery(~query=Queries.getQueryForProposalInfo(metaCycle))
  ->FutureJs.fromPromise(_err => None)
  ->Future.map(
      fun
      | Ok(proposalStats) when proposalStats |> Array.length > 0 =>
        proposalStats
        |> Array.map(proposal => {
             let proposalObj = proposal |> Obj.magic;
             let newProposal: MainType.proposalInfo = {
               count_operation_group_hash:
                 proposalObj##count_operation_group_hash,
               proposal: proposalObj##proposal,
             };
             newProposal;
           })
        |> toOption
      | Ok(_) => Some([||])
      | _err => None,
    )
  ->Future.get(callback);

// getNumBlocksApi = blocks = Queries.getQueryForNumBlocks
// getTransactionsApi = operations = getQueryForTransactionStats
// getFundraiserApi = operations = getQueryForFundraiserStats
// getTotalFundraiserApi = operations = getQueryForTotalFundraiserActivated
// getFeesStatsApi = operations = getQueryForFeesStats
// getBakersStatsApi = delegates = getQueryForBakerStats
// getMarketCapApi = accounts = getQueryForMarketCap
let getBlockInfoThunk =
    (~callback, ~metaCycle: int, ~timeStamp: float, ~config: MainType.config) =>
  Future.all([
    getForQueryApi(
      ~query=Queries.getQueryForNumBlocks(metaCycle, timeStamp),
      ~field="blocks",
      ~config,
    ),
    getForQueryApi(
      ~query=Queries.getQueryForTransactionStats(timeStamp),
      ~field="operations",
      ~config,
    ),
    getForQueryApi(
      ~query=Queries.getQueryForFundraiserStats(timeStamp),
      ~field="operations",
      ~config,
    ),
    getForQueryApi(
      ~query=Queries.getQueryForTotalFundraiserActivated(),
      ~field="operations",
      ~config,
    ),
    getForQueryApi(
      ~query=Queries.getQueryForFeesStats(timeStamp),
      ~field="operations",
      ~config,
    ),
    getForQueryApi(
      ~query=Queries.getQueryForBakerStats(),
      ~field="delegates",
      ~config,
    ),
    getForQueryApi(
      ~query=Queries.getQueryForMarketCap(),
      ~field="accounts",
      ~config,
    ),
  ])
  ->Future.map(
      fun
      | [
          Some(res1),
          Some(res2),
          Some(res3),
          Some(res4),
          Some(res5),
          Some(res6),
          Some(res7),
        ] => {
          let newTransInfoObj = res1 |> Obj.magic;
          let newTranInfo: MainType.transInfo = {
            countOriginatedContracts:
              newTransInfoObj##count_originated_contracts,
            countAmount: newTransInfoObj##count_amount,
            sumAmount: newTransInfoObj##sum_amount,
          };
          let newFundraiser = res2 |> Obj.magic;
          let newTotalFundraiser = res3 |> Obj.magic;
          let feesObj = res4 |> Obj.magic;
          let bakersObj = res5 |> Obj.magic;
          let marketObj = res6 |> Obj.magic;

          let newBlockInfo: MainType.blockInfo = {
            blockCount: (res7 |> Obj.magic)##count_hash,
            fundraiserCount: newFundraiser##count_kind,
            totalFundraiserCount: newTotalFundraiser##count_kind,
            sum_fee: feesObj##sum_fee,
            sum_consumed_gas: feesObj##sum_consumed_gas,
            num_bakers: bakersObj##count_pkh,
            bakers_sum_staking_balance: bakersObj##sum_staking_balance,
            totalTez: marketObj##sum_balance,
          };
          Some((newBlockInfo, newTranInfo));
        }
      | _ => None,
    )
  ->Future.get(callback);